#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <Arduino.h>
#include <WString.h>

void EEPROMupdate(int loc, byte val);

// Set to 1 if you are in the northern hemisphere.
#define NORTHERN_HEMISPHERE 1

// Time in ms between LCD screen updates during slewing operations
#define DISPLAY_UPDATE_TIME 200

// Make some variables in the sketch files available to the C++ code.
extern bool inSerialControl;
extern String version;
extern int PolarisRAHour;
extern int PolarisRAMinute;
extern int PolarisRASecond;
// Comment this out to save some code space
// #define DEBUG_MODE

// Uncomment to run a key diagnostic. No tracker functions are on at all.
// #define LCD_BUTTON_TEST

////////////////////////////////////////////////////////////////
//
// FEATURE SUPPORT SECTION
//
// Since the Arduino Uno has very little memory (32KB code, 2KB data) all features
// stretch the Uno a little too far. So in order to save memory we allow you to enable 
// and disable features to help manage memory usage.
// If you run the tracker with an Arduino Mega, you can uncomment all the features.
//
// If you would like to drive your OAT mount with only the LCD Shield, 
// you should comment out SUPPORT_SERIAL_CONTROL
//
// If you feel comfortable with configuring the OAT at startup manually, you should comment 
// out SUPPORT_GUIDED_STARTUP (maybe after you've used it for a while you know what to do).
//
// The POI menu can take a little data memory and you may not need it. If not, you can comment
// out SUPPORT_POINTS_OF_INTEREST
//
////////////////////////////////////////////////////////////////

// If you do not have a LCD shield on your Arduino Uno, uncomment the line below. This is
// useful if you are always going to run the mount from a laptop anyway.
// #define HEADLESS_CLIENT

#ifdef ESP8266
    #define HEADLESS_CLIENT
    #define WIFI_ENABLED 
    #define INFRA_SSID "yourSSID"
    #define INFRA_WPAKEY "yourWPAKey"
    #define OAT_WPAKEY "superSecret"
    #define HOSTNAME "OATerScope"
    // 0 - Infrastructure Only - Connecting to a Router
    // 1 - AP Mode Only        - Acting as a Router
    // 2 - Attempt Infrastructure, Fail over to AP Mode.
    #define WIFI_MODE 2 
#endif


// Uncomment this to enable the heating menu
// NOTE: Heating is currently not supported!
// #define SUPPORT_HEATING

#ifndef HEADLESS_CLIENT

    // Uncomment to support Guided Startup 
    #define SUPPORT_GUIDED_STARTUP

    // Uncomment to support full GO (was POI) menu. 
    // If this is commented out you still have a GO menu that has Home and Park.
    #define SUPPORT_POINTS_OF_INTEREST

    // Uncomment to support CTRL menu, allowing you to manually slew the mount with the buttons. 
    #define SUPPORT_MANUAL_CONTROL

    // Uncomment to support CAL menu, allowing you to calibrate various things
    #define SUPPORT_CALIBRATION

#endif
// Uncomment to support INFO menu that displays various pieces of information about the mount. 
// #define SUPPORT_INFO_DISPLAY

// Uncomment to support Serial Meade LX200 protocol support
// #define SUPPORT_SERIAL_CONTROL

// If we are making a headleass (no screen, no keyboard) client, always enable Serial.
#ifdef HEADLESS_CLIENT
#define SUPPORT_SERIAL_CONTROL
#endif

#endif
