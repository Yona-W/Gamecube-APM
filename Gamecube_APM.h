#ifndef GAMECUBE_OUTPUT_H
#define GAMECUBE_OUTPUT_H

#include <Gamecube_N64.h>
#include <Gamecube.h>
#include <EEPROM.h>
#include "analog_actions.h"


#define GCNPIN 13                   // The pin connected to the Gamecube / Gamecube Controller
#define SERIALLED 2                 // The pin connected to the "Serial port enabled" LED
#define RECLED 5                    // The pin connected to the "Recording enabled" LED

#define GCNPOLLRATE 120             // Gamecube input poll rate.
// Much, much more accurate than the millis() timer with low poll rates (interrupts disabled)

#define PRINT if(use_serial) Serial.print
#define PRINTLN if(use_serial) Serial.println

#define RESOLUTION 1000             // A new second will be reached every RESOLUTION milliseconds.


uint8_t *console_packet;            // The last data packet received from the console
uint8_t *controller_packet;         // The last data packet received from the controller

bool is_measuring;                  // Are we currently measuring the player's APM?
bool use_serial;                    // Are we using the serial connection?
bool use_frames;                    // Are we basing ourselves off the game's framerate or the internal millis() timer?

Gamecube_Report_t previous_report;  // The report returned by the controller before the current poll

long start_millis;                  // Milliseconds at the start of the recording
long current_frame;                 // The current frame / controller poll during this recording
uint8_t recording[1000];            // The data for the current recording. Each value is one second.

#endif
