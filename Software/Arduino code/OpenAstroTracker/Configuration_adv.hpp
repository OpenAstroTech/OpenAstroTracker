#pragma once

#include "Configuration.hpp"

// This is how many steps your stepper needs for a full rotation.
#if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  #define RA_STEPPER_SPR            4096  // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #define RA_STEPPER_SPEED          400   // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
  #define RA_STEPPER_ACCELERATION   600   // High speeds tend to make these cheap steppers unprecice
#elif RA_STEPPER_TYPE == STEPPER_TYPE_NEMA17
  #define RA_STEPPER_SPR            400   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #define RA_STEPPER_SPEED          1200  // You can change the speed and acceleration of the steppers here. Max. Speed = 3000. 
  #define RA_STEPPER_ACCELERATION   6000
#else
  #error New RA Stepper type? Add it here...
#endif

#if DEC_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  #define DEC_STEPPER_SPR            4096  // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #define DEC_STEPPER_SPEED          600   // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
  #define DEC_STEPPER_ACCELERATION   400   // High speeds tend to make these cheap steppers unprecice
#elif DEC_STEPPER_TYPE == STEPPER_TYPE_NEMA17
  #define DEC_STEPPER_SPR            400   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #define DEC_STEPPER_SPEED          1300  // You can change the speed and acceleration of the steppers here. Max. Speed = 3000. 
  #define DEC_STEPPER_ACCELERATION   6000
#else
  #error New DEC Stepper type? Add it here...
#endif

// the Circumference of the RA wheel.  V1 = 1057.1  |  V2 = 1131.0
#if RA_WHEEL_VERSION == 1
  #define RA_WHEEL_CIRCUMFERENCE 1057.1
#elif RA_WHEEL_VERSION >= 2
  #define RA_WHEEL_CIRCUMFERENCE 1131.0
#else
  #error Unsupported RA wheel version, please recheck RA_STEPPER_TYPE
#endif

// the Circumference of the RA wheel.  V1 = 1057.1  |  V2 = 1131
#define DEC_WHEEL_CIRCUMFERENCE 565.5

// Steps per degree:

// RA movement:
// The radius of the surface that the belt runs on (in V1 of the ring) was 168.24mm.
// Belt moves 40mm for one stepper revolution (2mm pitch, 20 teeth).
// RA wheel is 2 x PI x 168.24mm (V2:180mm) circumference = 1057.1mm (V2:1131mm)
// One RA revolution needs 26.43 (1057.1mm / 40mm) stepper revolutions (V2: 28.27 (1131mm/40mm))
// Which means 108245 steps (26.43 x 4096) moves 360 degrees (V2: 115812 steps (28.27 x 4096))
// So there are 300.1 steps/degree (108245 / 360)  (V2: 322 (115812 / 360))
// Theoretically correct RA tracking speed is 1.246586 (300 x 14.95903 / 3600) (V2 : 1.333800 (322 x 14.95903 / 3600) steps/sec (this is for 20T)
#define RA_STEPS_PER_DEGREE   (RA_WHEEL_CIRCUMFERENCE / (RA_PULLEY_TEETH * 2.0) * RA_STEPPER_SPR / 360.0)

// DEC movement
// Belt moves 40mm for one stepper revolution (2mm pitch, 20 teeth).
// DEC wheel is 2 x PI x 90mm circumference which is 565.5mm
// One DEC revolution needs 14.13 (565.5mm/40mm) stepper revolutions
// Which means 57907 steps (14.14 x 4096) moves 360 degrees
// So there are 160.85 steps/degree (57907/360) (this is for 20T)
#define DEC_STEPS_PER_DEGREE  (DEC_WHEEL_CIRCUMFERENCE / (DEC_PULLEY_TEETH * 2.0) * DEC_STEPPER_SPR / 360.0)


// MICROSTEPPING
// Only affects NEMA steppers!
// Only affects calculations, Microstepping is set by MS pins, 
// !!EXCEPT for TMC2209 UART where this value actually sets the SLEW microstepping
// Valid values: 1, 2, 4, 8, 16, 32, 64, 128, 256
// !! Values greater or equal 32 are not supported by each driver, TMC2209 supports these values.
#if RA_STEPPER_TYPE == STEPPER_TYPE_NEMA17
#define SET_MICROSTEPPING 8        
#endif

// TMC2209 UART settings
// These settings work only with TMC2209 in UART connection (single wire to TX)
#define TRACKING_MICROSTEPPING 64  // Set the MS mode for tracking only. Slew MS is set by SET_MICROSTEPPING above

#define RA_RMSCURRENT 1200       // RMS current in mA. Warning: Peak current will be 1.414 times higher!! Do not exceed your steppers max current!
#define RA_STALL_VALUE 100       // adjust this value if the RA autohoming sequence often false triggers, or triggers too late

#define DEC_SLEW_MICROSTEPPING  16  // The microstep mode used for slewing DEC
#define DEC_GUIDE_MICROSTEPPING 64  // The microstep mode used for Guiding DEC only
#define DEC_STALL_VALUE 10    // adjust this value if the RA autohoming sequence often false triggers, or triggers too late
#define DEC_RMSCURRENT 1000   // RMS current in mA. Warning: Peak current will be 1.414 times higher!! Do not exceed your steppers max current!
#define DEC_HOLDCURRENT 20    // [0, ... , 31] x/32 of the run current when standing still. 0=1/32... 31=32/32
#define USE_AUTOHOME 0        // Autohome with TMC2209 stall detection:  ON = 1  |  OFF = 0   
//                  ^^^ leave at 0 for now, doesnt work properly yet
#define RA_AUDIO_FEEDBACK  0 // If one of these are set to 1, the respective driver will shut off the stealthchop mode, resulting in a audible whine
#define DEC_AUDIO_FEEDBACK 0 // of the stepper coils. Use this to verify that UART is working properly. 


////////////////////////////
//
// GUIDE SETTINGS
// This is the multiplier of the normal trackingspeed that a pulse will have 
// NEMA steppers only! Doesnt affect 28BY (which is hardcoded to 2x and 0)
// Note that the East trackingspeed is calculated as the multiplier-1.0
// Standard value: RA 1.5;  DEC 1.0
#define RA_PULSE_MULTIPLIER 1.5
#define DEC_PULSE_MULTIPLIER 1.0


////////////////////////////
//
// INVERT AXIS
// Set to 1 or 0 to invert motor directions
#ifndef RA_INVERT_DIR 
#define RA_INVERT_DIR 0 
#endif
#ifndef DEC_INVERT_DIR 
#define DEC_INVERT_DIR 0
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                  ////////
// LCD SETTINGS     ////////
//                  ////////
////////////////////////////
// 
// UPDATE TIME
// Time in ms between LCD screen updates during slewing operations
#define DISPLAY_UPDATE_TIME 200


////////////////////////////
//
// HEADLESS CLIENT
// If you do not have a LCD shield on your Arduino Mega, set this to 1 on the line below. This is
// useful if you are always going to run the mount from a laptop anyway.
#ifndef HEADLESS_CLIENT
#define HEADLESS_CLIENT 0
#endif

#ifdef __AVR_ATmega328P__   // UNO must use headless
#undef HEADLESS_CLIENT
#define HEADLESS_CLIENT 1
#endif

////////////////////////////
//
// LCD BUTTON TEST
// Set this to 1 to run a key diagnostic. No tracker functions are on at all.
#define LCD_BUTTON_TEST 0


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                     ///
// HARDWARE EXTENSIONS SUPPORT SECTION ///
//                                     ///
//////////////////////////////////////////
//
// Enable Azimuth and Altitude motor functionality in Configuration.hpp
#define AZIMUTH_MAX_SPEED 500
#define AZIMUTH_MAX_ACCEL 200
#define AZIMUTH_ARC_SECONDS_PER_STEP (7.9997f)
#define AZIMUTH_STEPS_PER_ARC_MINUTE (60.0f/AZIMUTH_ARC_SECONDS_PER_STEP)

#define ALTITUDE_MAX_SPEED 500
#define ALTITUDE_MAX_ACCEL 200
#define ALTITUDE_ARC_SECONDS_PER_STEP (0.61761f)
#define ALTITUDE_STEPS_PER_ARC_MINUTE (60.0f/ALTITUDE_ARC_SECONDS_PER_STEP)

#if HEADLESS_CLIENT == 0 // <-- Ignore this line
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                         ///
// FEATURE SUPPORT SECTION ///
//                         ///
//////////////////////////////
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
//
// Set this to 1 to support full GO menu. 
// If this is set to 0 you still have a GO menu that has Home and Park.
  #define SUPPORT_POINTS_OF_INTEREST   1

// Set this to 1 to support Guided Startup 
  #define SUPPORT_GUIDED_STARTUP       1

// Set this to 1 to support CTRL menu, allowing you to manually slew the mount with the buttons. 
  #define SUPPORT_MANUAL_CONTROL       1

// Set this to 1 to support CAL menu, allowing you to calibrate various things
  #define SUPPORT_CALIBRATION          1

// Set this to 1 to support INFO menu that displays various pieces of information about the mount. 
  #define SUPPORT_INFO_DISPLAY         1

// Set this to 1 to support Serial Meade LX200 protocol support
  #define SUPPORT_SERIAL_CONTROL       1

#endif  // HEADLESS_CLIENT <-- Ignore this    



#if defined(ESP8266) || defined(ESP32) // <-- ignore this line
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                //////////
// WIFI SETTINGS  //////////
//                //////////
////////////////////////////
// These settings are valid only for ESP8266 or ESP32
//
// Define some things, dont change: ///
#define ESPBOARD
#undef HEADLESS_CLIENT
#define HEADLESS_CLIENT 1
#define WIFI_ENABLED 
#if defined(ESP8266) 
  #undef RUN_STEPPERS_IN_MAIN_LOOP
  #define RUN_STEPPERS_IN_MAIN_LOOP 1
#endif
///////////////////////////////////////
//
// SETTINGS
//
  #define INFRA_SSID "YouSSID"
  #define INFRA_WPAKEY "YourWPAKey"
  #define OAT_WPAKEY "superSecret"
  #define HOSTNAME "OATerScope"

  #define WIFI_MODE 2 
  // 0 - Infrastructure Only - Connecting to a Router
  // 1 - AP Mode Only        - Acting as a Router
  // 2 - Attempt Infrastructure, Fail over to AP Mode.
  
#endif // End WIFI SETTINGS


// This is set to 1 for boards that do not support interrupt timers
#define RUN_STEPPERS_IN_MAIN_LOOP 0


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                //////////
// DEBUG OPTIONS  //////////
//                //////////
////////////////////////////
// Debugging output control
// Each bit in the debug level specifies a kind of debug to enable. Do not change.
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

////////////////////////////
//
// DEBUG OUTPUT
//
#define DEBUG_LEVEL (DEBUG_NONE)
// #define DEBUG_LEVEL (DEBUG_SERIAL|DEBUG_WIFI|DEBUG_INFO|DEBUG_MOUNT|DEBUG_GENERAL)
// #define DEBUG_LEVEL (DEBUG_ANY)
// #define DEBUG_LEVEL (DEBUG_INFO|DEBUG_MOUNT|DEBUG_GENERAL)
//
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




////////////////////////////
// ERRORS

#if DEC_HOLDCURRENT < 1 || DEC_HOLDCURRENT > 31
#error "Holdcurrent has to be within 1 and 31!"
#endif
#if RA_RMSCURRENT > 2000 || DEC_RMSCURRENT > 2000
#error "Do you really want to set the RMS motorcurrent above 2 Ampere? Thats almost 3A peak! Delete this error if you know what youre doing" 
#endif
#if RA_STEPPER_TYPE != STEPPER_TYPE_28BYJ48 && ESP32
#error "Sorry, ESP32 only supports 28BYJ48 steppers at this moment"
#endif
#if RA_STEPPER_TYPE != STEPPER_TYPE_28BYJ48 && __AVR_ATmega328P__
#error "Sorry, Arduino Uno only supports 28BYJ48 steppers."
#endif



////////////////////////////
// Misc stuff, ignore

#if HEADLESS_CLIENT == 1 
#define SUPPORT_SERIAL_CONTROL 1
#endif

// Set this to 1 this to enable the heating menu
// NOTE: Heating is currently not supported!
#define SUPPORT_HEATING 0
