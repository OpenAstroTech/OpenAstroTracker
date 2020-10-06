/*
* This Configuration file contains basic settings. Use Configuration_adv for
* more advanced adjustments! 
*/

#pragma once

#include "Constants.hpp"

/**
 * We support local configurations so that you can setup it up once with your hardware and pinout
 * and not need to worry about a new version from Git overwriting your setup. 
 * There are multiple ways to define a local config file:
 *  - For all boards/hardware configs
 *    Create a file called configuration_local.hpp (best to copy configuration_sample_local.hpp and 
 *    change it as needed)
 *  - Specific to a board
 *    Create a file called configuration_local_<board>.hpp, where <board> is either 'mega' or 
 *    'esp32' or (here, too, best to copy Configuration_sample_local.hpp and change it as needed). 
 *    The code automatically picks the right one at compile time. This is useful if you are 
 *    developer or just have multiple OATs. 
 *  - Specific to a hardware config
 *    Create a file called configuration_local_<foo>.hpp, where <foo> is whatever you want to use
 *    to identify that setup. Then uncomment the lines 45 and 46 below. This is useful if have 
 *    multiple OATs. 
 * 
 * These files won't be tracked by Git and thus will remain after branch changes or code updates. 
  **/

#if defined(ESP32)                  // ESP32
    #define LOCAL_CONFIG "Configuration_local_esp32.hpp"
    #if !__has_include(LOCAL_CONFIG)
        #undef LOCAL_CONFIG 
        #define LOCAL_CONFIG "Configuration_local.hpp"
    #endif
#elif defined(__AVR_ATmega2560__)   // Arduino Mega
    #define LOCAL_CONFIG "Configuration_local_mega.hpp"
    #if !__has_include(LOCAL_CONFIG)
        #undef LOCAL_CONFIG 
        #define LOCAL_CONFIG "Configuration_local.hpp"
    #endif
#else                               // Other board?
    #define LOCAL_CONFIG "Configuration_local.hpp"
#endif

// Uncomment these two lines to override the automatic local config with yours.
// #undef LOCAL_CONFIG 
// #define LOCAL_CONFIG "Configuration_local_foo.hpp"

#if __has_include(LOCAL_CONFIG)
#include LOCAL_CONFIG
#endif

// Set to 1 for the northern hemisphere, 0 otherwise
#ifndef NORTHERN_HEMISPHERE
#define NORTHERN_HEMISPHERE 1
#endif

// Used RA wheel version. Unless you printed your OAT before March 2020, you're using 
// a version 2 or higher (software only differentiates between 1 and more than 1)
#ifndef RA_WHEEL_VERSION
#define RA_WHEEL_VERSION 4
#endif

// Stepper types/models. See supported stepper values. Change according to the steppers you are using
#ifndef RA_STEPPER_TYPE
#define RA_STEPPER_TYPE     STEPPER_TYPE_28BYJ48
#endif
#ifndef DEC_STEPPER_TYPE
#define DEC_STEPPER_TYPE    STEPPER_TYPE_28BYJ48
#endif

// Driver selection
// GENERIC drivers include A4988 and any Bipolar STEP/DIR based drivers. Note Microstep assignments in config_pins.
#ifndef RA_DRIVER_TYPE
#define RA_DRIVER_TYPE      DRIVER_TYPE_ULN2003
#endif
#ifndef DEC_DRIVER_TYPE
#define DEC_DRIVER_TYPE     DRIVER_TYPE_ULN2003
#endif

// Your pulley tooth count. 16 for the bought (aluminium) one, 20 for the printed one.
#ifndef RA_PULLEY_TEETH
#define RA_PULLEY_TEETH     16
#endif
#ifndef DEC_PULLEY_TEETH
#define DEC_PULLEY_TEETH    16
#endif

// Set this to 1 if you are using a NEO6m GPS module for HA/LST and location automatic determination.
// GPS uses Serial1 by default, which is pins 18/19 on Mega. Change in configuration_adv.hpp
// If supported, download the library https://github.com/mikalhart/TinyGPSPlus/releases and extract it to C:\Users\*you*\Documents\Arduino\libraries
#ifndef USE_GPS
#define USE_GPS 0
#endif

// Set this to 1 if you are using a MPU6050 electronic level
// Wire the board to 20/21 on Mega. Change pins in configuration_pins.hpp if you use other pins
#ifndef USE_GYRO_LEVEL
#define USE_GYRO_LEVEL 0
#endif

// Set this to 1 if your gyro is mounted such that roll and pitch are in the wrong direction
#ifndef GYRO_AXIS_SWAP
#define GYRO_AXIS_SWAP 1
#endif

// Set this to 1 if the mount has motorized Azimuth and Altitude adjustment. Set pins in configuration_pins.hpp. Change motor speeds in Configuration_adv.hpp
#ifndef AZIMUTH_ALTITUDE_MOTORS
#define AZIMUTH_ALTITUDE_MOTORS  0
#endif

// These values are needed to calculate the current position during initial alignment.
// Use something like Stellarium to look up the RA of Polaris in JNow (on date) variant.
// This changes slightly over weeks, so adjust every couple of months.
// This value is from 13.Aug.2020, next adjustment suggested at end 2020
// The same could be done for the DEC coordinates but they dont change significantly for the next 5 years
#ifndef POLARIS_RA_HOUR
#define POLARIS_RA_HOUR     2
#endif
#ifndef POLARIS_RA_MINUTE
#define POLARIS_RA_MINUTE   58
#endif
#ifndef POLARIS_RA_SECOND
#define POLARIS_RA_SECOND   34
#endif