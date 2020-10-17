#pragma once

/**
 * This file contains constants that SHOULD NOT BE CHANGED by oat users!
 * If you are a developer and want to add new hardware support, add a
 * proper definition here with increased value.
 **/

/**
 * Supported display types. Use one of these values for DISPLAY_TYPE configuration matching your used display.
 * 
 * DISPLAY_TYPE_NONE:           No display. Use this if you don't use any display.
 * DISPLAY_TYPE_LCD_KEYPAD:     1602 LCD Keypad shield which can be mounted directly to an Arduino UNO / Mega boards.
 *                              Example: https://www.digikey.com/en/products/detail/dfrobot/DFR0009/7597118
 **/
#define DISPLAY_TYPE_NONE       0
#define DISPLAY_TYPE_LCD_KEYPAD 1

// Supported stepper models
#define STEPPER_TYPE_28BYJ48    0
#define STEPPER_TYPE_NEMA17     1

// Supported stepper driver models
#define DRIVER_TYPE_ULN2003             0
#define DRIVER_TYPE_GENERIC             1
#define DRIVER_TYPE_TMC2209_STANDALONE  2
#define DRIVER_TYPE_TMC2209_UART        3
