// If you really want to look through this code, i apologise for my terrible coding
//#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <AccelStepper.h>
//#include <avr8-stub.h>
#include <LiquidCrystal.h>

#include "Utility.h"
#include "DayTime.hpp"
#include "Mount.hpp"
#include "MeadeCommandProcessor.h"


#define HALFSTEP 8
#define FULLSTEP 4

//SoftwareSerial BT(10,11);

#ifdef ESP8266
// RA Motor pins
#define RAmotorPin1  D0    // IN1 auf ULN2003 driver 1    //  2 / 22
#define RAmotorPin3  D1    // IN2 auf ULN2003 driver 1    //  3 / 24d
#define RAmotorPin2  D2    // IN3 auf ULN2003 driver 1   // 11 / 26
#define RAmotorPin4  D3    // IN4 auf ULN2003 driver 1   // 12 / 28

// DEC Motor pins
#define DECmotorPin1  D5    // IN1 auf ULN2003 driver 2
#define DECmotorPin3  D6    // IN3 auf ULN2003 driver 2
#define DECmotorPin2  D7    // IN2 auf ULN2003 driver 2
#define DECmotorPin4  D8    // IN4 auf ULN2003 driver 2

// ST4 Input Pins - TODO.
#define st4North      SD0 
#define st4South      SD1
#define st4West       SD2
#define st4East       SD3
#endif

#ifdef __AVR_ATmega328P__ // normal Arduino Mapping
#define RAmotorPin1  2    // IN1 auf ULN2003 driver 1    //  2 / 22
#define RAmotorPin3  3    // IN2 auf ULN2003 driver 1    //  3 / 24
#define RAmotorPin2  11    // IN3 auf ULN2003 driver 1   // 11 / 26
#define RAmotorPin4  12    // IN4 auf ULN2003 driver 1   // 12 / 28

// DEC Motor pins
#define DECmotorPin1  15    // IN1 auf ULN2003 driver 2
#define DECmotorPin3  16    // IN3 auf ULN2003 driver 2
#define DECmotorPin2  17    // IN2 auf ULN2003 driver 2
#define DECmotorPin4  18    // IN4 auf ULN2003 driver 2
#endif

#ifdef __AVR_ATmega2560__  // Arduino Mega
#define RAmotorPin1  22    // IN1 auf ULN2003 driver 1    //  2 / 22
#define RAmotorPin3  24    // IN2 auf ULN2003 driver 1    //  3 / 24
#define RAmotorPin2  26    // IN3 auf ULN2003 driver 1   // 11 / 26
#define RAmotorPin4  28    // IN4 auf ULN2003 driver 1   // 12 / 28

#define DECmotorPin1  30    // IN1 auf ULN2003 driver 2
#define DECmotorPin3  32    // IN3 auf ULN2003 driver 2
#define DECmotorPin2  34    // IN2 auf ULN2003 driver 2
#define DECmotorPin4  36    // IN4 auf ULN2003 driver 2
#endif

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

#ifdef SUPPORT_GUIDED_STARTUP
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

#ifdef SUPPORT_HEATING
// HEAT menu variables
int heatselect;   // Which stepper are we changing?
int RAheat = 0;   // Are we heating the RA stepper?
int DECheat = 0;  // Are we heating the DEC stepper?
#endif

//debugging
String inBT;
