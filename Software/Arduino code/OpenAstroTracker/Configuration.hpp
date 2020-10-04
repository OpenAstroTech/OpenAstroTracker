#pragma once
#include "lib/util/debug.hpp"
#include "Configuration_adv.hpp"   // <-- 
#include "Configuration_pins.hpp"  // <-- ignore these
/*
  =======================================================================================================================================

    Version 1.8

    1. Connect your Arduino, under tools choose the board you're using (e.g. Arduino Mega), set the right Port and set "Arduino ISP" as the Programmer.
    2. Hit upload (Ctrl-U)

    You might need to download the "AccelStepper" library by Mike McCauley

    Authors: /u/intercipere
             /u/clutchplate
             /u/EorEquis

  =======================================================================================================================================
*/

#define Version "V2.0.00h"

///////////////////////////////////////////////////////////////////////////
// Also use Configuration_adv for further adjustments!
///////////////////////////////////////////////////////////////////////////

// See NORTHERN_HEMISPHERE in Configuration_adv.hpp if you NOT in the northern hemisphere

///////////////////////////////////////////////////////////////////////////
// RA stepper                                                            //
///////////////////////////////////////////////////////////////////////////
// This is how many steps your stepper needs for a full rotation.
#if RA_STEPPER_TYPE == STEP_28BYJ48
  #define RAStepsPerRevolution  4096    // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #define RAspeed                400    // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
  #define RAacceleration         600    // High speeds tend to make these cheap steppers unprecice
#elif RA_STEPPER_TYPE == STEP_NEMA17
  #define RAStepsPerRevolution   400    // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #define RAspeed               1200    // You can change the speed and acceleration of the steppers here. Max. 
  #define RAacceleration        1000
#else
  #error New RA Stepper type? Define steps per stepper pulley revolution here...
#endif

// Your RA pulley tooth count:
#define RAPulleyTeeth 16

// the Circumference of the RA wheel.  V1 = 1057.1  |  V2 = 1131 
#define RACircumference  1131.0


///////////////////////////////////////////////////////////////////////////
// DEC stepper                                                           //
///////////////////////////////////////////////////////////////////////////
// This is how many steps your stepper needs for a full rotation.
#if DEC_STEPPER_TYPE == STEP_28BYJ48
  #define DECStepsPerRevolution   4096   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #define DECspeed                 600   // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
  #define DECacceleration          400   // High speeds tend to make these cheap steppers unprecice
#elif DEC_STEPPER_TYPE == STEP_NEMA17
  #define DECStepsPerRevolution    400   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  #define DECspeed                1300   // You can change the speed and acceleration of the steppers here.
  #define DECacceleration         1000   
#else
  #error New DEC Stepper type? Define steps per stepper pulley revolution here...
#endif

// Your DEC pulley tooth count:
#define DecPulleyTeeth  16
#define DEC_Circumference  565.5f


// These values are needed to calculate the current position during initial alignment.
// Use something like Stellarium to look up the RA of Polaris in JNow (on date) variant.
// This changes slightly over weeks, so adjust every couple of months.
// This value is from 13.Aug.2020, next adjustment suggested at end 2020
// The same could be done for the DEC coordinates but they dont change significantly for the next 5 years
#define PolarisRAHour   2
#define PolarisRAMinute 57
#define PolarisRASecond 27

// Some explanations:

// RA movement:
// The radius of the surface that the belt runs on (in V1 of the ring) was 168.24mm.
// Belt moves 40mm for one stepper revolution (2mm pitch, 20 teeth).
// RA wheel is 2 x PI x 168.24mm (V2:180mm) circumference = 1057.1mm (V2:1131mm)
// One RA revolution needs 26.43 (1057.1mm / 40mm) stepper revolutions (V2: 28.27 (1131mm/40mm))
// Which means 108245 steps (26.43 x 4096) moves 360 degrees (V2: 115812 steps (28.27 x 4096))
// So there are 300.1 steps/degree (108245 / 360)  (V2: 322 (115812 / 360))
// Theoretically correct RA tracking speed is 1.246586 (300 x 14.95903 / 3600) (V2 : 1.333800 (322 x 14.95903 / 3600) steps/sec (this is for 20T)

// DEC movement
// Belt moves 40mm for one stepper revolution (2mm pitch, 20 teeth).
// DEC wheel is 2 x PI x 90mm circumference which is 565.5mm
// One DEC revolution needs 14.13 (565.5mm/40mm) stepper revolutions
// Which means 57907 steps (14.14 x 4096) moves 360 degrees
// So there are 160.85 steps/degree (57907/360) (this is for 20T)

// Some calculations, ignore 
#define RAStepsPerDegree ((RACircumference / (RAPulleyTeeth * 2.0f) * RAStepsPerRevolution / 360.0f))
#define DECStepsPerDegree ((DEC_Circumference / (DecPulleyTeeth * 2.0f) * DECStepsPerRevolution / 360.0f))


/////////////////////////////////////////////////////////////////////////////////////
//
// IGNORE THE FOLLOWING AREA
//
/////////////////////////////////////////////////////////////////////////////////////
#if (RA_DRIVER_TYPE == TMC2209_UART) || (DEC_DRIVER_TYPE == TMC2209_UART)
  #include <TMCStepper.h>
#endif

#if USE_GPS == 1
  #include <TinyGPS++.h>
  extern TinyGPSPlus gps;
#endif

#define HALFSTEP_MODE 8
#define FULLSTEP_MODE 4
#define DRIVER_MODE 1

////////////////////////////////////
// Stepper definitions /////////////
#if RA_STEPPER_TYPE == STEP_28BYJ48
  // RA Motor pins
  #if INVERT_RA_DIR == 1
    #define RAmotorPin1  RA_IN1_PIN
    #define RAmotorPin3  RA_IN2_PIN
    #define RAmotorPin2  RA_IN3_PIN
    #define RAmotorPin4  RA_IN4_PIN
  #else
    #define RAmotorPin1  RA_IN4_PIN
    #define RAmotorPin3  RA_IN3_PIN
    #define RAmotorPin2  RA_IN2_PIN
    #define RAmotorPin4  RA_IN1_PIN
  #endif
#elif RA_STEPPER_TYPE == STEP_NEMA17
    #define RAmotorPin1  RA_STEP_PIN
    #define RAmotorPin2  RA_DIR_PIN
#endif

// DEC Motor pins
#if DEC_STEPPER_TYPE == STEP_28BYJ48
  #if INVERT_DEC_DIR == 1
    #define DECmotorPin1  DEC_IN4_PIN
    #define DECmotorPin3  DEC_IN3_PIN
    #define DECmotorPin2  DEC_IN2_PIN
    #define DECmotorPin4  DEC_IN1_PIN
  #else
    #define DECmotorPin1  DEC_IN1_PIN
    #define DECmotorPin3  DEC_IN2_PIN
    #define DECmotorPin2  DEC_IN3_PIN
    #define DECmotorPin4  DEC_IN4_PIN
  #endif
#elif DEC_STEPPER_TYPE == STEP_NEMA17
    #define DECmotorPin1  DEC_STEP_PIN
    #define DECmotorPin2  DEC_DIR_PIN
#endif

#if AZIMUTH_ALTITUDE_MOTORS == 1
    #define AZmotorPin1  AZ_IN1_PIN    
    #define AZmotorPin3  AZ_IN2_PIN    
    #define AZmotorPin2  AZ_IN3_PIN    
    #define AZmotorPin4  AZ_IN4_PIN    

    #define ALTmotorPin1  ALT_IN1_PIN 
    #define ALTmotorPin3  ALT_IN2_PIN 
    #define ALTmotorPin2  ALT_IN3_PIN 
    #define ALTmotorPin4  ALT_IN4_PIN 
#endif
// End Stepper Definitions //////////////
/////////////////////////////////////////

/////////////////////////////////////////
// Driver definitions ///////////////////
#if RA_DRIVER_TYPE == TMC2209_STANDALONE
  //#define RA_EN_PIN 40  // Enable Pin
#endif
#if (RA_DRIVER_TYPE == TMC2209_UART) || (DEC_DRIVER_TYPE == TMC2209_UART)
  #define R_SENSE 0.11f           // 0.11 for StepStick
#endif
// End Driver Definitions ///////////////
/////////////////////////////////////////

extern bool inStartup;        // Start with a guided startup

// Serial control variables
extern bool inSerialControl; // When the serial port is in control

//// Variables for use in the CONTROL menu
//extern bool inControlMode;  // Is manual control enabled

// Global variables
// bool isUnreachable = false;
