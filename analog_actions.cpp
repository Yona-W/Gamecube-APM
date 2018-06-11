#include "analog_actions.h"

uint8_t get_stick_active(Gamecube_Report_t report){
  int stick_x = report.xAxis - 128;
  int stick_y = report.yAxis - 128;
  int distanceSqr = stick_x * stick_x + stick_y * stick_y;
  return distanceSqr > 5000;
}

uint8_t get_cstick_active(Gamecube_Report_t report){
  int stick_x = report.cxAxis - 128;
  int stick_y = report.cyAxis - 128;
  int distanceSqr = stick_x * stick_x + stick_y * stick_y;
  return distanceSqr > 5000;
}

uint8_t get_l_active(Gamecube_Report_t report){
  return report.left > 50;
}

uint8_t get_r_active(Gamecube_Report_t report){
  return report.right > 50;
}