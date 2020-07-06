#ifndef _GLOBALS_HPP_
#define _GLOBALS_HPP_

#include <Arduino.h>
#include <WString.h>

// Set to 1 if you are in the northern hemisphere.
#define NORTHERN_HEMISPHERE 1

// Time in ms between LCD screen updates during slewing operations
#define DISPLAY_UPDATE_TIME 200

// Stepper selection ///////////
#define RA_Stepper_TYPE  0     // 28BYJ-48 = 0   |   NEMA = 1
#define DEC_Stepper_TYPE  0    // 28BYJ-48 = 0   |   NEMA = 1

// Make some variables in the sketch files available to the C++ code.
extern bool inSerialControl;
extern String version;
extern byte PolarisRAHour;
extern byte PolarisRAMinute;
extern byte PolarisRASecond;

// Debugging output control
// Each bit in the debug level specifies a kind of debug to enable. 
#define DEBUG_NONE           0x00
#define DEBUG_INFO           0x01
#define DEBUG_SERIAL         0x02
#define DEBUG_WIFI           0x04
#define DEBUG_MOUNT          0x08
#define DEBUG_MOUNT_VERBOSE  0x10
#define DEBUG_GENERAL        0x20
#define DEBUG_MEADE          0x40
#define DEBUG_VERBOSE        0x80
#define DEBUG_ANY            0xFF

// Bit Name                 Output
//  0  DEBUG_INFO           General output, like startup variables and status
//  1  DEBUG_SERIAL         Serial commands and replies
//  2  DEBUG_WIFI           Wifi related output
//  3  DEBUG_MOUNT          Mount processing output
//  4  DEBUG_MOUNT_VERBOSE  Verbose mount processing (coordinates, etc)
//  5  DEBUG_GENERAL        Other misc. output
//  6  DEBUG_MEADE          Meade command handling output

// Set this to specify the amount of debug output OAT should send to the serial port.
// Note that if you use an app to control OAT, ANY debug output will likely confuse that app.
// Debug output is useful if you are using Wifi to control the OAT or if you are issuing
// manual commands via a terminal.
//
// #define DEBUG_LEVEL (DEBUG_SERIAL|DEBUG_WIFI|DEBUG_INFO|DEBUG_MOUNT|DEBUG_GENERAL)
// #define DEBUG_LEVEL (DEBUG_ANY)
// #define DEBUG_LEVEL (DEBUG_INFO|DEBUG_MOUNT|DEBUG_GENERAL)
#define DEBUG_LEVEL (DEBUG_NONE)

// Set this to 1 to run a key diagnostic. No tracker functions are on at all.
#define LCD_BUTTON_TEST 0

// Set to 1 to reverse the direction of RA motor
#define INVERT_RA_DIR 0

// Set to 1 to reverse the direction of DEC motor
#define INVERT_DEC_DIR 0

// If you do not have a LCD shield on your Arduino Uno, set this to 1 on the line below. This is
// useful if you are always going to run the mount from a laptop anyway.
#define HEADLESS_CLIENT 0

// This is set to 1 for boards that do not support interrupt timers
#define RUN_STEPPERS_IN_MAIN_LOOP 0

#if defined(ESP8266) || defined(ESP32)
    #define ESPBOARD
    #undef HEADLESS_CLIENT
    #define HEADLESS_CLIENT 1
    #define WIFI_ENABLED 
    #define INFRA_SSID "YouSSID"
    #define INFRA_WPAKEY "YourWPAKey"
    #define OAT_WPAKEY "superSecret"
    #define HOSTNAME "OATerScope"
    // 0 - Infrastructure Only - Connecting to a Router
    // 1 - AP Mode Only        - Acting as a Router
    // 2 - Attempt Infrastructure, Fail over to AP Mode.
    #define WIFI_MODE 2 
    #if defined(ESP8266) 
        #undef RUN_STEPPERS_IN_MAIN_LOOP
        #define RUN_STEPPERS_IN_MAIN_LOOP 1
    #endif
#endif


////////////////////////////////////////////////////////////////
//
// FEATURE SUPPORT SECTION
//
// Since the Arduino Uno has very little memory (32KB code, 2KB data) all features
// stretch the Uno a little too far. So in order to save memory we allow you to enable 
// and disable features to help manage memory usage.
// If you run the tracker with an Arduino Mega, you can set all the features to 1.
//
// If you would like to drive your OAT mount with only the LCD Shield, or are on a Uno,
// you should set SUPPORT_SERIAL_CONTROL to 0
//
// If you feel comfortable with configuring the OAT at startup manually, you should set
// SUPPORT_GUIDED_STARTUP to 0 (maybe after you've used it for a while you know what to do).
//
// The POI menu can take a little data memory and you may not need it. If not, you can set
// SUPPORT_POINTS_OF_INTEREST to 0
//
////////////////////////////////////////////////////////////////

// Set this to 1 this to enable the heating menu
// NOTE: Heating is currently not supported!
#define SUPPORT_HEATING 0

#if HEADLESS_CLIENT == 0

    // Set this to 1 to support Guided Startup 
    #define SUPPORT_GUIDED_STARTUP 1

    // Set this to 1 to support full GO (was POI) menu. 
    // If this is set to 0 you still have a GO menu that has Home and Park.
    #define SUPPORT_POINTS_OF_INTEREST 1

    // Set this to 1 to support CTRL menu, allowing you to manually slew the mount with the buttons. 
    #define SUPPORT_MANUAL_CONTROL 1

    // Set this to 1 to support CAL menu, allowing you to calibrate various things
    #define SUPPORT_CALIBRATION 1

    // Set this to 1 to support INFO menu that displays various pieces of information about the mount. 
    #define SUPPORT_INFO_DISPLAY 1

    // Set this to 1 to support Serial Meade LX200 protocol support
    #define SUPPORT_SERIAL_CONTROL 1

#else
    // If we are making a headleass (no screen, no keyboard) client, always enable Serial.
    #define SUPPORT_SERIAL_CONTROL 1
#endif  // HEADLESS_CLIENT

#endif // _GLOBALS_HPP
