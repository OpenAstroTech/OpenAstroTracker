#pragma once

/*
* This file contains constants that SHOULD NOT BE CHANGED by oat users!
* If you are a developer and want to add new hardware support, add a
* proper definition here with increased value.
*/

// Supported display types
#define DISPLAY_TYPE_NONE           0
#define DISPLAY_TYPE_LCD_KEYPAD     1

// Supported stepper models
#define STEPPER_TYPE_28BYJ48  0
#define STEPPER_TYPE_NEMA17   1

// Supported stepper driver models
#define DRIVER_TYPE_ULN2003              0
#define DRIVER_TYPE_GENERIC              1
#define DRIVER_TYPE_TMC2209_STANDALONE   2
#define DRIVER_TYPE_TMC2209_UART         3

