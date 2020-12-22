#pragma once

#include "Configuration.hpp"

/**
 * This file contains advanced configurations. Edit values here only if you know what you are doing. Invalid values
 * can lead to OAT misbehaving very bad and in worst case could even lead to hardware damage. The default values here
 * were chosen after many tests and can are currently concidered to work the best.
 **/

// This is how many steps your stepper needs for a full rotation.
#if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  #ifndef RA_STEPPER_SPR
    #define RA_STEPPER_SPR            4096  // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #endif
  #ifndef RA_STEPPER_SPEED
    #define RA_STEPPER_SPEED          400   // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
  #endif
  #ifndef RA_STEPPER_ACCELERATION
    #define RA_STEPPER_ACCELERATION   600   // High speeds tend to make these cheap steppers unprecice
  #endif
#elif RA_STEPPER_TYPE == STEPPER_TYPE_NEMA17
  #ifndef RA_STEPPER_SPR
    #define RA_STEPPER_SPR            400   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #endif
  #ifndef RA_STEPPER_SPEED
    #define RA_STEPPER_SPEED          1200  // You can change the speed and acceleration of the steppers here. Max. Speed = 3000. 
  #endif
  #ifndef RA_STEPPER_ACCELERATION
    #define RA_STEPPER_ACCELERATION   6000
  #endif
#else
  #error New RA Stepper type? Add it here...
#endif

#if DEC_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  #ifndef DEC_STEPPER_SPR
    #define DEC_STEPPER_SPR            4096  // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #endif
  #ifndef DEC_STEPPER_SPEED
    #define DEC_STEPPER_SPEED          600   // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
   #endif
  #ifndef DEC_STEPPER_ACCELERATION
    #define DEC_STEPPER_ACCELERATION   400   // High speeds tend to make these cheap steppers unprecice
  #endif
#elif DEC_STEPPER_TYPE == STEPPER_TYPE_NEMA17
  #ifndef DEC_STEPPER_SPR
    #define DEC_STEPPER_SPR            400   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #endif
  #ifndef DEC_STEPPER_SPEED
    #define DEC_STEPPER_SPEED          1300  // You can change the speed and acceleration of the steppers here. Max. Speed = 3000. 
  #endif
  #ifndef DEC_STEPPER_ACCELERATION
    #define DEC_STEPPER_ACCELERATION   6000
  #endif
#else
  #error New DEC Stepper type? Add it here...
#endif

// the Circumference of the RA wheel.  V1 = 1057.1  |  V2 = 1131.0
#if RA_WHEEL_VERSION == 1
  #define RA_WHEEL_CIRCUMFERENCE 1057.1
#elif RA_WHEEL_VERSION >= 2
  #define RA_WHEEL_CIRCUMFERENCE 1132.73
#else
  #error Unsupported RA wheel version, please recheck RA_STEPPER_TYPE
#endif

// the Circumference of the DEC wheel.
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

#if AZ_DRIVER_TYPE == DRIVER_TYPE_ULN2003
  #define AZ_MICROSTEPPING        2     // Halfstep mode using ULN2003 driver
#elif AZ_DRIVER_TYPE == DRIVER_TYPE_GENERIC || AZ_DRIVER_TYPE == DRIVER_TYPE_TMC2209_STANDALONE || AZ_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
  #define AZ_MICROSTEPPING        32
#endif
#if AZ_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  #define AZ_STEPPER_SPR            2048  // 28BYJ-48 in full step mode
  #define AZ_STEPPER_SPEED          600   // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
  #define AZ_STEPPER_ACCELERATION   400   // High speeds tend to make these cheap steppers unprecice
#elif AZ_STEPPER_TYPE == STEPPER_TYPE_NEMA17
  #define AZ_STEPPER_SPR            400   // NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #define AZ_STEPPER_SPEED          600  // You can change the speed and acceleration of the steppers here. Max. Speed = 3000. 
  #define AZ_STEPPER_ACCELERATION   1000
#else
  #error New Stepper type? Add it here...
#endif


#if ALT_DRIVER_TYPE == DRIVER_TYPE_ULN2003
  #define ALT_MICROSTEPPING        1     // Fullstep mode using ULN2003 driver
#elif ALT_DRIVER_TYPE == DRIVER_TYPE_GENERIC || ALT_DRIVER_TYPE == DRIVER_TYPE_TMC2209_STANDALONE || ALT_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
  #define ALT_MICROSTEPPING        32
#endif
#if ALT_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  #define ALT_STEPPER_SPR            2048  // 28BYJ-48 in full step mode
  #define ALT_STEPPER_SPEED          600   // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
  #define ALT_STEPPER_ACCELERATION   400   // High speeds tend to make these cheap steppers unprecice
#elif ALT_STEPPER_TYPE == STEPPER_TYPE_NEMA17
  #define ALT_STEPPER_SPR            400   // NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #define ALT_STEPPER_SPEED          600  // You can change the speed and acceleration of the steppers here. Max. Speed = 3000. 
  #define ALT_STEPPER_ACCELERATION   1000
#else
  #error New Stepper type? Add it here...
#endif


// the Circumference of the AZ rotation. 808mm dia.
#define AZ_CIRCUMFERENCE 2538.4
// the Circumference of the AZ rotation. 770mm dia.
#define ALT_CIRCUMFERENCE 2419
// the ratio of the ALT gearbox (40:3)
#define ALT_WORMGEAR_RATIO (40/3)

#define AZIMUTH_STEPS_PER_REV           (AZ_CORRECTION_FACTOR * (AZ_CIRCUMFERENCE / (AZ_PULLEY_TEETH * 2)) * AZ_STEPPER_SPR)
#define ALTITUDE_STEPS_PER_REV          (ALT_CORRECTION_FACTOR * (ALT_CIRCUMFERENCE / (ALT_PULLEY_TEETH * 2)) * ALT_STEPPER_SPR * ALT_WORMGEAR_RATIO)
#define AZIMUTH_STEPS_PER_ARC_MINUTE    (AZIMUTH_STEPS_PER_REV / (360 * 60.0f)) // Used to determine move distance in steps
#define ALTITUDE_STEPS_PER_ARC_MINUTE   (ALTITUDE_STEPS_PER_REV / (360 * 60.0f)) // Used to determine move distance in steps

// ALT/AZ TMC2209 UART settings
// These settings work only with TMC2209 in UART connection (single wire to TX)
#define AZ_AUDIO_FEEDBACK 0 // of the stepper coils. Use this to verify that UART is working properly. 
#define ALT_AUDIO_FEEDBACK 0 // of the stepper coils. Use this to verify that UART is working properly.
#define AZ_STALL_VALUE 10    // adjust this value if the RA autohoming sequence often false triggers, or triggers too late
#define AZ_RMSCURRENT 1000   // RMS current in mA. Warning: Peak current will be 1.414 times higher!! Do not exceed your steppers max current!
#define ALT_STALL_VALUE 10    // adjust this value if the RA autohoming sequence often false triggers, or triggers too late
#define ALT_RMSCURRENT 1000   // RMS current in mA. Warning: Peak current will be 1.414 times higher!! Do not exceed your steppers max current!



// Enable dew heater output (for boards that have MOSFETs)
#define DEW_HEATER 0


#if DISPLAY_TYPE > 0

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

#endif  // DISPLAY_TYPE

// Enable Meade protocol communication over serial
#define SUPPORT_SERIAL_CONTROL 1


#if defined(ESP32)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                //////////
// WIFI SETTINGS  //////////
//                //////////
////////////////////////////
// These settings are valid only for ESP32
//
// Define some things, dont change: ///
#define ESPBOARD
// #define BLUETOOTH_ENABLED
#define WIFI_ENABLED 
///////////////////////////////////////
//
// SETTINGS
//
  #define INFRA_SSID "YourSSID"
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
#define DEBUG_NONE           0x0000
#define DEBUG_INFO           0x0001
#define DEBUG_SERIAL         0x0002
#define DEBUG_WIFI           0x0004
#define DEBUG_MOUNT          0x0008
#define DEBUG_MOUNT_VERBOSE  0x0010
#define DEBUG_GENERAL        0x0020
#define DEBUG_MEADE          0x0040
#define DEBUG_VERBOSE        0x0080
#define DEBUG_STEPPERS       0x0100
#define DEBUG_EEPROM         0x0200
#define DEBUG_GYRO           0x0400
#define DEBUG_ANY            0xFFFF

////////////////////////////
//
// DEBUG OUTPUT
//
#define DEBUG_LEVEL (DEBUG_NONE)
// #define DEBUG_LEVEL (DEBUG_STEPPERS|DEBUG_MOUNT)
// #define DEBUG_LEVEL (DEBUG_INFO|DEBUG_MOUNT|DEBUG_GENERAL)
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
