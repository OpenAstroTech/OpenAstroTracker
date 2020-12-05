#pragma once

/**
 * This file contains constants that SHOULD NOT BE CHANGED by oat users!
 * If you are a developer and want to add new hardware support, add a
 * proper definition here with increased value.
 **/

/**
 * Supported display types. Use one of these values for DISPLAY_TYPE configuration matching your used display.
 * 
 * DISPLAY_TYPE_NONE:                       No display. Use this if you don't use any display.
 * DISPLAY_TYPE_LCD_KEYPAD:                 1602 LCD Keypad shield which can be mounted directly to an Arduino UNO / Mega boards.
 *                                          Example: https://www.digikey.com/en/products/detail/dfrobot/DFR0009/7597118
 * DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23008:    RGB LCD Keypad shield based on the MCP23008 I/O Expander. It can be mounted 
 *                                          directly to an Arduino UNO / Mega boards and controlled over I2C.
 * DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23017:    RGB LCD Keypad shield based on the MCP23017 I/O Expander. It can be mounted 
 *                                          directly to an Arduino UNO / Mega boards and controlled over I2C.                                         
 * DISPLAY_TYPE_LCD_JOY_I2C_SSD1306:        I2C 32x128 OLED display module with SSD1306 controller, plus mini joystick
 *                                          Display: https://www.banggood.com/Geekcreit-0_91-Inch-128x32-IIC-I2C-Blue-OLED-LCD-Display-DIY-Module-SSD1306-Driver-IC-DC-3_3V-5V-p-1140506.html
 *                                          Joystick: https://www.banggood.com/3pcs-JoyStick-Module-Shield-2_54mm-5-pin-Biaxial-Buttons-Rocker-for-PS2-Joystick-Game-Controller-Sensor-p-1586026.html
 **/
#define DISPLAY_TYPE_NONE                       0
#define DISPLAY_TYPE_LCD_KEYPAD                 1
#define DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23008    2
#define DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23017    3
#define DISPLAY_TYPE_LCD_JOY_I2C_SSD1306        4


// Supported stepper models
#define STEPPER_TYPE_28BYJ48    0
#define STEPPER_TYPE_NEMA17     1


// Supported stepper driver models
#define DRIVER_TYPE_ULN2003             0
#define DRIVER_TYPE_GENERIC             1
#define DRIVER_TYPE_TMC2209_STANDALONE  2
#define DRIVER_TYPE_TMC2209_UART        3
