#include "Gamecube_APM.h"

void setup() {
  // Setup pins
  pinMode(GCNPIN, INPUT);
  pinMode(SERIALLED, OUTPUT);
  pinMode(RECLED, OUTPUT);
  digitalWrite(SERIALLED, 1);
  digitalWrite(RECLED, 1);

  // Setup globals
  for(int i = 0; i < 1000; i++){
    recording[i] = 0;
  }
  console_packet = calloc(3, sizeof(uint8_t));
  controller_packet = calloc(8, sizeof(uint8_t));

  // Open serial line
  Serial.begin(256000);
}

void loop() {
  // Get data from console / controller
  gc_n64_receive_raw(GCNPIN, console_packet, 3, controller_packet, 8);

  //Set status LEDs
  digitalWrite(SERIALLED, use_serial);
  digitalWrite(RECLED, !is_measuring);

  // Only proceed if data packet seems well formed
  if(console_packet[0] == 0x40){
    Gamecube_Report_t controller_report = *(Gamecube_Report_t*)controller_packet;
    if(is_measuring)
      measure(controller_report);

    /* Checking for the controller key combinations.
     *  All inputs must be performed while holding the X, Y, Z buttons:
     *  
     *  D-Pad up:       Begin writing to serial port
     *  D-Pad down:     Stop writing to serial port
     *  D-Pad left:     Begin measuring APM using console poll rate (for real Gamecube)
     *  D-Pad left + R: Begin measuring APM using internal clock (for adapter)
     *  D-Pad right:    Stop measuring APM
     *  
     *  A:              Output all stored stats through serial port
     *  Start:          Clear stored games
     */
    
    if(!use_serial && controller_report.dup && controller_report.z && controller_report.x && controller_report.y && !previous_report.dup){
      use_serial = true;
      Serial.println("Serial connection will be used for live stats.\nPlease do not disconnect.");
    }

    if(use_serial && controller_report.ddown && controller_report.z && controller_report.x && controller_report.y && !previous_report.ddown){
      use_serial = false;
      Serial.println("Serial connection disabled for stats.\nSafe to disconnect.");
    }
    
    if(!is_measuring && controller_report.dleft && !controller_report.r && controller_report.z && controller_report.x && controller_report.y && !previous_report.dleft){
      PRINTLN("Started measuring (GameCube console, 120Hz)");
      use_frames = true;
      begin_measuring();
    }

    if(!is_measuring && controller_report.dleft && controller_report.r && controller_report.z && controller_report.x && controller_report.y && !previous_report.dleft){
      PRINTLN("Started measuring (GCN Controller Adapter, Arduino millisecond timer)");
      use_frames = false;
      begin_measuring();
    }

    if(is_measuring && controller_report.dright && controller_report.z && controller_report.x && controller_report.y && !previous_report.dright){
      PRINTLN("Done measuring");
      end_measuring();
    }

    if(!is_measuring && controller_report.a && controller_report.z && controller_report.x && controller_report.y &&!previous_report.a){
      print_storage();
      Serial.flush();
    }

    if(controller_report.start && controller_report.z && controller_report.x  && controller_report.y &&!previous_report.start){
      reset_storage();
      PRINTLN("Cleared storage");
    }
    
    previous_report = controller_report;
  }
}

void calculate_stats(){
  int highest_apm_3sec = 0;
  int highest_apm_10sec = 0;
  int average_apm = 0;
  int most_inputs_in_second = 0;

  int last_second;
  
  if(use_frames){
    last_second = current_frame / GCNPOLLRATE;
  }
  else{
    last_second = (millis() - start_millis) / RESOLUTION;
  }

  // Loop to calculate the stats from the array of actions per second
  for(int i = 0; i < last_second; i++){
    average_apm += recording[i];
    
    if(recording[i] > most_inputs_in_second) most_inputs_in_second = recording[i];

    if(i > 2){
      int apm_3sec = 0;
      for(int j = i - 3; j < i; j++){
        apm_3sec += recording[j];
      }
      apm_3sec *= 20;
      if(apm_3sec > highest_apm_3sec) highest_apm_3sec = apm_3sec; 
    }

    if(i > 9){
      int apm_10sec = 0;
      for(int j = i - 10; j < i; j++){
        apm_10sec += recording[j];
      }
      apm_10sec *= 6;
      if(apm_10sec > highest_apm_10sec) highest_apm_10sec = apm_10sec; 
    }
  }

  average_apm /= (last_second / 60);

  PRINTLN("Recording processed!");
  PRINT("Duration (seconds):");
  PRINTLN(last_second);
  PRINT("Highest APM (3-second burst): ");
  PRINTLN(highest_apm_3sec);
  PRINT("Highest APM (10-second average): ");
  PRINTLN(highest_apm_10sec);
  PRINT("Average APM (whole game average): ");
  PRINTLN(average_apm);
  PRINT("Most inputs in one second: ");
  PRINTLN(most_inputs_in_second);

  save(last_second, highest_apm_3sec, highest_apm_10sec, average_apm, most_inputs_in_second);
  
}

void begin_measuring(){
  for(int i = 0; i < 1000; i++){
    recording[i] = 0;
  }
  is_measuring = true;
  current_frame = 0;
  start_millis = millis();
}

void end_measuring(){
  is_measuring = false;
  calculate_stats();
}

void measure(Gamecube_Report_t controller_report){
    current_frame++;
    int current_actions = 0;
  
    if(controller_report.a && !previous_report.a){
      current_actions++;
    }
    if(controller_report.b && !previous_report.b){
      current_actions++;
    }
    if(controller_report.x && !previous_report.x){
      current_actions++;
    }
    if(controller_report.y && !previous_report.y){
      current_actions++;
    }
    if(controller_report.z && !previous_report.z){
      current_actions++;
    }
    if(get_stick_active(controller_report) && !get_stick_active(previous_report)){
      current_actions++;
    }
     if(get_cstick_active(controller_report) && !get_cstick_active(previous_report)){
      current_actions++;
    }
     if(get_l_active(controller_report) && !get_l_active(previous_report)){
      current_actions++;
    }
     if(get_r_active(controller_report) && !get_r_active(previous_report)){
      current_actions++;
    }
    
    if(current_actions) update_second(current_actions);
    
}

// Update the current second of the recording with the number of inputs performed this second
void update_second(int actions){
  int current_second;
  if(use_frames){
    current_second = current_frame / GCNPOLLRATE;
  }
  else{
    current_second = (millis() - start_millis) / RESOLUTION;
  }
  
  // Make sure we don't overflow
  if(current_second == 1000)
    end_measuring();

  if(recording[current_second] == 0){
    PRINT(current_second);
    PRINT(":");
    PRINTLN(recording[current_second]);
  }

  recording[current_second] += actions;
}

void reset_storage(){
  EEPROM.write(0, 0);
  EEPROM.write(1, 2);
}

void save(int last_second, int highest_apm_3sec, int highest_apm_10sec, int average_apm, int most_inputs_in_second){

  int offset = word(EEPROM.read(0), EEPROM.read(1));

  if(offset > 999){
    PRINTLN("Storage full, unable to save");
  }
  
  EEPROM.write(offset, highByte(last_second));
  EEPROM.write(offset + 1, lowByte(last_second));
  EEPROM.write(offset + 2, highByte(highest_apm_3sec));
  EEPROM.write(offset + 3, lowByte(highest_apm_3sec));
  EEPROM.write(offset + 4, highByte(highest_apm_10sec));
  EEPROM.write(offset + 5, lowByte(highest_apm_10sec));
  EEPROM.write(offset + 6, highByte(average_apm));
  EEPROM.write(offset + 7, lowByte(average_apm));
  EEPROM.write(offset + 8, highByte(most_inputs_in_second));
  EEPROM.write(offset + 9, lowByte(most_inputs_in_second));

  offset += 10;
  EEPROM.write(0, highByte(offset));
  EEPROM.write(1, lowByte(offset));

  PRINTLN("Saved recording");
  
}

void print_storage(){
  int max_offset = word(EEPROM.read(0), EEPROM.read(1));
  for(int i = 2; i < max_offset; i += 10){
    PRINT("Recording ");
    PRINTLN((i - 2) / 10);
    PRINT("Duration (seconds):");
    PRINTLN(word(EEPROM.read(i), EEPROM.read(i + 1)));
    PRINT("Highest APM (3-second burst): ");
    PRINTLN(word(EEPROM.read(i + 2), EEPROM.read(i + 3)));
    PRINT("Highest APM (10-second average): ");
    PRINTLN(word(EEPROM.read(i + 4), EEPROM.read(i + 5)));
    PRINT("Average APM (whole game average): ");
    PRINTLN(word(EEPROM.read(i + 6), EEPROM.read(i + 7)));
    PRINT("Most inputs in one second: ");
    PRINTLN(word(EEPROM.read(i + 8), EEPROM.read(i + 9))); 
    PRINTLN();
  }
}
