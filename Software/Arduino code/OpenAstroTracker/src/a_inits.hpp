#pragma once

#include <AccelStepper.h>
#if HEADLESS_CLIENT == 0
#include <LiquidCrystal.h>
#endif
#include "inc/Globals.hpp"

#include "Utility.hpp"
#include "DayTime.hpp"
#include "Mount.hpp"
#include "MeadeCommandProcessor.hpp"

#if (RA_DRIVER_TYPE == TMC2209_UART) || (DEC_DRIVER_TYPE == TMC2209_UART)
  #include <TMCStepper.h>
#endif

#if USE_GPS == 1
  //#include <SoftwareSerial.h>
  #include <TinyGPS++.h>

  //SoftwareSerial SoftSerial(GPS_SERIAL_RX_PIN, GPS_SERIAL_TX_PIN); // RX, TX
  TinyGPSPlus gps;
#endif

#define HALFSTEP_MODE 8
#define FULLSTEP_MODE 4
#define DRIVER_MODE 1

////////////////////////////////////
// Stepper definitions /////////////
#if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  // RA Motor pins
  #if RA_INVERT_DIR == 1
    #define RAmotorPin1  RA_IN1_PIN    // IN1 auf ULN2003 driver 1
    #define RAmotorPin3  RA_IN2_PIN    // IN2 auf ULN2003 driver 1
    #define RAmotorPin2  RA_IN3_PIN    // IN3 auf ULN2003 driver 1
    #define RAmotorPin4  RA_IN4_PIN    // IN4 auf ULN2003 driver 1
  #else
    #define RAmotorPin1  RA_IN4_PIN    // IN1 auf ULN2003 driver 1
    #define RAmotorPin3  RA_IN3_PIN    // IN2 auf ULN2003 driver 1
    #define RAmotorPin2  RA_IN2_PIN    // IN3 auf ULN2003 driver 1
    #define RAmotorPin4  RA_IN1_PIN    // IN4 auf ULN2003 driver 1
  #endif
#elif RA_STEPPER_TYPE == STEPPER_TYPE_NEMA17
    #define RAmotorPin1  RA_STEP_PIN
    #define RAmotorPin2  RA_DIR_PIN
#endif

// DEC Motor pins
#if DEC_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  #if DEC_INVERT_DIR == 1
    #define DECmotorPin1  DEC_IN4_PIN    // IN1 auf ULN2003 driver 2
    #define DECmotorPin3  DEC_IN3_PIN    // IN2 auf ULN2003 driver 2
    #define DECmotorPin2  DEC_IN2_PIN    // IN3 auf ULN2003 driver 2
    #define DECmotorPin4  DEC_IN1_PIN    // IN4 auf ULN2003 driver 2
  #else
    #define DECmotorPin1  DEC_IN1_PIN    // IN1 auf ULN2003 driver 2
    #define DECmotorPin3  DEC_IN2_PIN    // IN2 auf ULN2003 driver 2
    #define DECmotorPin2  DEC_IN3_PIN    // IN3 auf ULN2003 driver 2
    #define DECmotorPin4  DEC_IN4_PIN    // IN4 auf ULN2003 driver 2
  #endif
#elif DEC_STEPPER_TYPE == STEPPER_TYPE_NEMA17
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
#if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_STANDALONE
  //#define RA_EN_PIN 40  // Enable Pin
#endif
#if (RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART) || (DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART)
  #define R_SENSE 0.11f           // 0.11 for StepStick
#endif
// End Driver Definitions ///////////////
/////////////////////////////////////////


// Menu IDs
#define RA_Menu 1
#define DEC_Menu 2
#define HA_Menu 3
#define Heat_Menu 4
#define Calibration_Menu 5
#define Control_Menu 6
#define Home_Menu 7
#define POI_Menu 8
#define Status_Menu 9

// How many menu items at most?
#define MAXMENUITEMS 10

#if SUPPORT_GUIDED_STARTUP == 1
bool inStartup = true;        // Start with a guided startup
#else
bool inStartup = false;        // Start with a guided startup
#endif

// Serial control variables
bool inSerialControl = false; // When the serial port is in control
bool quitSerialOnNextButtonRelease = false; // Used to detect SELECT button to quit Serial mode.

//// Variables for use in the CONTROL menu
bool inControlMode = false;  // Is manual control enabled

// Global variables
bool isUnreachable = false;

// RA variables
int RAselect;

// DEC variables
int DECselect;

// HA variables
int HAselect;

#if SUPPORT_HEATING == 1
// HEAT menu variables
int heatselect;   // Which stepper are we changing?
int RAheat = 0;   // Are we heating the RA stepper?
int DECheat = 0;  // Are we heating the DEC stepper?
#endif



//debugging
String inBT;
