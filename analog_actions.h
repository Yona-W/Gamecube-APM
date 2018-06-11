#ifndef ANALOG_ACTIONS_H
#define ANALOG_ACTIONS_H
#include "Gamecube.h"
uint8_t get_stick_active(Gamecube_Report_t report);
uint8_t get_cstick_active(Gamecube_Report_t report);
uint8_t get_l_active(Gamecube_Report_t report);
uint8_t get_r_active(Gamecube_Report_t report);

#endif
