#pragma once

//#include <SoftwareSerial.h>
#include <AccelStepper.h>
#include <LiquidCrystal.h>

#include "Utility.hpp"
#include "DayTime.hpp"
#include "Mount.hpp"
#include "MeadeCommandProcessor.hpp"
#if RA_Driver_TYPE == 3 // if using TMC UART
  #include <TMCStepper.h>
#endif

#define HALFSTEP 8
#define FULLSTEP 4
#define DRIVER 1

///////////////////////////////////////////////////////////////////////////
// ESP8266 Wifi Board (NodeMCU)
///////////////////////////////////////////////////////////////////////////
#ifdef ESP8266
// RA Motor pins
  #if INVERT_RA_DIR == 1
    #define RAmotorPin1  D0    // IN1 auf ULN2003 driver 1
    #define RAmotorPin3  D1    // IN2 auf ULN2003 driver 1
    #define RAmotorPin2  D2    // IN3 auf ULN2003 driver 1
    #define RAmotorPin4  D3    // IN4 auf ULN2003 driver 1
  #else
    #define RAmotorPin1  D3    // IN1 auf ULN2003 driver 1
    #define RAmotorPin3  D2    // IN2 auf ULN2003 driver 1
    #define RAmotorPin2  D1    // IN3 auf ULN2003 driver 1
    #define RAmotorPin4  D0    // IN4 auf ULN2003 driver 1
  #endif

// DEC Motor pins
  #if INVERT_DEC_DIR == 1
    #define DECmotorPin1  D5    // IN1 auf ULN2003 driver 2
    #define DECmotorPin3  D6    // IN3 auf ULN2003 driver 2
    #define DECmotorPin2  D7    // IN2 auf ULN2003 driver 2
    #define DECmotorPin4  D8    // IN4 auf ULN2003 driver 2
  #else
    #define DECmotorPin1  D8    // IN1 auf ULN2003 driver 2
    #define DECmotorPin3  D7    // IN3 auf ULN2003 driver 2
    #define DECmotorPin2  D6    // IN2 auf ULN2003 driver 2
    #define DECmotorPin4  D5    // IN4 auf ULN2003 driver 2
  #endif

// ST4 Input Pins - TODO.
    #define st4North      SD0 
    #define st4South      SD1
    #define st4West       SD2
    #define st4East       SD3
#endif


///////////////////////////////////////////////////////////////////////////
// ESP32 Wifi Board
///////////////////////////////////////////////////////////////////////////
#ifdef ESP32
// RA Motor pins
  #if INVERT_RA_DIR == 1
    #define RAmotorPin1  35    // IN1 auf ULN2003 driver 1
    #define RAmotorPin3  34    // IN2 auf ULN2003 driver 1
    #define RAmotorPin2  39    // IN3 auf ULN2003 driver 1
    #define RAmotorPin4  36    // IN4 auf ULN2003 driver 1
  #else
    #define RAmotorPin1  23    // IN1 auf ULN2003 driver 1
    #define RAmotorPin3  22    // IN2 auf ULN2003 driver 1
    #define RAmotorPin2  21    // IN3 auf ULN2003 driver 1
    #define RAmotorPin4  19    // IN4 auf ULN2003 driver 1
  #endif

// DEC Motor pins
  #if INVERT_DEC_DIR == 1
    #define DECmotorPin1  26    // IN1 auf ULN2003 driver 2
    #define DECmotorPin3  25    // IN3 auf ULN2003 driver 2
    #define DECmotorPin2  33    // IN2 auf ULN2003 driver 2
    #define DECmotorPin4  32    // IN4 auf ULN2003 driver 2
  #else
    #define DECmotorPin1  18    // IN1 auf ULN2003 driver 2
    #define DECmotorPin3  5     // IN3 auf ULN2003 driver 2
    #define DECmotorPin2  17    // IN2 auf ULN2003 driver 2
    #define DECmotorPin4  16    // IN4 auf ULN2003 driver 2
  #endif

// ST4 Input Pins - TODO.
    /*#define st4North      SD0 
    #define st4South      SD1
    #define st4West       SD2
    #define st4East       SD3*/
#endif

///////////////////////////////////////////////////////////////////////////
// Arduino Uno
///////////////////////////////////////////////////////////////////////////
#ifdef __AVR_ATmega328P__ // normal Arduino Mapping
#if RA_Stepper_TYPE == 0  // 28BYJ
// RA Motor pins
  #if INVERT_RA_DIR == 1
    #define RAmotorPin1  12    // IN1 auf ULN2003 driver 1
    #define RAmotorPin3  11    // IN2 auf ULN2003 driver 1
    #define RAmotorPin2  3     // IN3 auf ULN2003 driver 1
    #define RAmotorPin4  2     // IN4 auf ULN2003 driver 1
  #else
    #define RAmotorPin1  2     // IN1 auf ULN2003 driver 1
    #define RAmotorPin3  3     // IN2 auf ULN2003 driver 1
    #define RAmotorPin2  11    // IN3 auf ULN2003 driver 1
    #define RAmotorPin4  12    // IN4 auf ULN2003 driver 1
  #endif
#endif
#if RA_Stepper_TYPE == 1  // NEMA
    #define RAmotorPin1 11
    #define RAmotorPin2 12
#endif

// DEC Motor pins
#if DEC_Stepper_TYPE == 0  // 28BYJ
  #if INVERT_DEC_DIR == 1
    #define DECmotorPin1  18    // IN1 auf ULN2003 driver 2
    #define DECmotorPin2  16    // IN2 auf ULN2003 driver 2
    #define DECmotorPin3  17    // IN3 auf ULN2003 driver 2
    #define DECmotorPin4  15    // IN4 auf ULN2003 driver 2
  #else
    #define DECmotorPin1  15    // IN1 auf ULN2003 driver 2
    #define DECmotorPin2  17    // IN2 auf ULN2003 driver 2
    #define DECmotorPin3  16    // IN3 auf ULN2003 driver 2
    #define DECmotorPin4  18    // IN4 auf ULN2003 driver 2
  #endif
#endif
#if DEC_Stepper_TYPE == 1  // NEMA
    #define DECmotorPin1  16
    #define DECmotorPin2  17
#endif

#endif

///////////////////////////////////////////////////////////////////////////
// Arduino Mega
///////////////////////////////////////////////////////////////////////////
#ifdef __AVR_ATmega2560__  // Arduino Mega
#if RA_Stepper_TYPE == 0  // 28BYJ
// RA Motor pins
  #if INVERT_RA_DIR == 1
    #define RAmotorPin1  22    // IN1 auf ULN2003 driver 1
    #define RAmotorPin3  24    // IN2 auf ULN2003 driver 1
    #define RAmotorPin2  26    // IN3 auf ULN2003 driver 1
    #define RAmotorPin4  28    // IN4 auf ULN2003 driver 1
  #else
    #define RAmotorPin1  28    // IN1 auf ULN2003 driver 1
    #define RAmotorPin3  26    // IN2 auf ULN2003 driver 1
    #define RAmotorPin2  24    // IN3 auf ULN2003 driver 1
    #define RAmotorPin4  22    // IN4 auf ULN2003 driver 1
  #endif
#endif
#if RA_Stepper_TYPE == 1  // NEMA
    #define RAmotorPin1  RA_STEP_PIN
    #define RAmotorPin2  RA_DIR_PIN
#endif

// DEC Motor pins
#if DEC_Stepper_TYPE == 0  // 28BYJ
  #if INVERT_DEC_DIR == 1
    #define DECmotorPin1  36    // IN1 auf ULN2003 driver 2
    #define DECmotorPin3  34    // IN2 auf ULN2003 driver 2
    #define DECmotorPin2  32    // IN3 auf ULN2003 driver 2
    #define DECmotorPin4  30    // IN4 auf ULN2003 driver 2
  #else
    #define DECmotorPin1  30    // IN1 auf ULN2003 driver 2
    #define DECmotorPin3  32    // IN2 auf ULN2003 driver 2
    #define DECmotorPin2  34    // IN3 auf ULN2003 driver 2
    #define DECmotorPin4  36    // IN4 auf ULN2003 driver 2
  #endif
#endif
#if DEC_Stepper_TYPE == 1  // NEMA
    #define DECmotorPin1  DEC_STEP_PIN
    #define DECmotorPin2  DEC_DIR_PIN
#endif
  
#endif

// Driver definitions /////////////
#if RA_Driver_TYPE == 2
  #define RA_EN_PIN 40  // Enable Pin
#endif
#if RA_Driver_TYPE == 3
  //#define RA_EN_PIN 40
  //#define RA_DIAG_PIN 52
  //#define RA_SERIAL_PORT Serial3  // HardwareSerial port
  //#define RA_DRIVER_ADDRESS 0b00  // Set by MS1/MS2
  //#define R_SENSE 0.11f           // 0.11 for StepStick
  //#define RA_STALL_VALUE 100
  //#define RA_RMSCURRENT 500
#endif
#if DEC_Driver_TYPE == 3
  /*#define DEC_EN_PIN 41
  #define DEC_MS1_PIN 44
  #define DEC_SERIAL_PORT Serial2  // HardwareSerial port
  #define DEC_DRIVER_ADDRESS 0b01  // Set by MS1/MS2 (MS1 HIGH, MS2 LOW)
  #define R_SENSE 0.11f           // 0.11 for StepStick
  #define DEC_STALL_VALUE 100
  #define DEC_RMSCURRENT 500*/
#endif
// End Driver Definitions //////////////


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
