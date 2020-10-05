#pragma once
#include "Constants.hpp"
/*
* This Configuration file contains basic settings. Use Configuration_adv for
* more advanced adjustments! 
*/

#define NORTHERN_HEMISPHERE 1
// Set to 1 for the northern hemisphere, 0 otherwise

// Used RA wheel version. Unless you printed your OAT before March 2020, you're using 
// a version 2 or higher (software only differentiates between 1 and more than 1)
#define RA_WHEEL_VERSION 4

// Stepper types/models. See supported stepper values. Change according to the steppers you are using
#define RA_STEPPER_TYPE     STEPPER_TYPE_28BYJ48
#define DEC_STEPPER_TYPE    STEPPER_TYPE_28BYJ48

// Driver selection
// GENERIC drivers include A4988 and any Bipolar STEP/DIR based drivers. Note Microstep assignments in config_pins.
#define RA_DRIVER_TYPE      DRIVER_TYPE_ULN2003
#define DEC_DRIVER_TYPE     DRIVER_TYPE_ULN2003

// Your pulley tooth count. 16 for the bought (aluminium) one, 20 for the printed one.
#define RA_PULLEY_TEETH     16
#define DEC_PULLEY_TEETH    16

// Set this to 1 if you are using a NEO6m GPS module for HA/LST and location automatic determination.
// GPS uses Serial1 by default, which is pins 18/19 on Mega. Change in configuration_adv.hpp
#define USE_GPS 0
// If supported, download the library https://github.com/mikalhart/TinyGPSPlus/releases and extract it to C:\Users\*you*\Documents\Arduino\libraries

// Set this to 1 if you are using a MPU6050 electronic level
// Wire the board to 20/21 on Mega. Change pins in configuration_pins.hpp if you use other pins
#define USE_GYRO_LEVEL 0
#define GYRO_AXIS_SWAP 1  // Set this to 1 if your gyro is mounted such that roll and pitch are in the wrong direction

// Set this to 1 if the mount has motorized Azimuth and Altitude adjustment. Set pins in configuration_pins.hpp. Change motor speeds in Configuration_adv.hpp
#define AZIMUTH_ALTITUDE_MOTORS  0

// These values are needed to calculate the current position during initial alignment.
// Use something like Stellarium to look up the RA of Polaris in JNow (on date) variant.
// This changes slightly over weeks, so adjust every couple of months.
// This value is from 13.Aug.2020, next adjustment suggested at end 2020
// The same could be done for the DEC coordinates but they dont change significantly for the next 5 years
#define POLARIS_RA_HOUR     2
#define POLARIS_RA_MINUTE   58
#define POLARIS_RA_SECOND   34