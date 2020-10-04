#pragma once
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

String version = "V1.8.27";

///////////////////////////////////////////////////////////////////////////
// Also use Configuration_adv for further adjustments!
///////////////////////////////////////////////////////////////////////////

// See NORTHERN_HEMISPHERE in Configuration_adv.hpp if you NOT in the northern hemisphere

///////////////////////////////////////////////////////////////////////////
// RA stepper                                                            //
///////////////////////////////////////////////////////////////////////////
// This is how many steps your stepper needs for a full rotation.
#if RA_STEPPER_TYPE == STEP_28BYJ48
  float RAStepsPerRevolution = 4096;   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  int RAspeed = 400;          // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
  int RAacceleration = 600;   // High speeds tend to make these cheap steppers unprecice
#elif RA_STEPPER_TYPE == STEP_NEMA17
  float RAStepsPerRevolution = 400;   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  int RAspeed = 1200;          // You can change the speed and acceleration of the steppers here. Max. 
  int RAacceleration = 6000;
#else
  #error New RA Stepper type? Define steps per stepper pulley revolution here...
#endif

// Your RA pulley tooth count:
int RAPulleyTeeth = 16;

// the Circumference of the RA wheel.  V1 = 1057.1  |  V2 = 1131 
float RACircumference = 1131.0;


///////////////////////////////////////////////////////////////////////////
// DEC stepper                                                           //
///////////////////////////////////////////////////////////////////////////
// This is how many steps your stepper needs for a full rotation.
#if DEC_STEPPER_TYPE == STEP_28BYJ48
  float DECStepsPerRevolution = 4096;   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  int DECspeed = 600;          // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
  int DECacceleration = 400;    // High speeds tend to make these cheap steppers unprecice
#elif DEC_STEPPER_TYPE == STEP_NEMA17
  float DECStepsPerRevolution = 400;   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200
  int DECspeed = 1300;           // You can change the speed and acceleration of the steppers here.
  int DECacceleration = 6000;   
#else
  #error New DEC Stepper type? Define steps per stepper pulley revolution here...
#endif

// Your DEC pulley tooth count:
int DecPulleyTeeth = 16;
float DEC_Circumference = 565.5;



// These values are needed to calculate the current position during initial alignment.
// Use something like Stellarium to look up the RA of Polaris in JNow (on date) variant.
// This changes slightly over weeks, so adjust every couple of months.
// This value is from 13.Aug.2020, next adjustment suggested at end 2020
// The same could be done for the DEC coordinates but they dont change significantly for the next 5 years
byte PolarisRAHour = 2;
byte PolarisRAMinute = 58;
byte PolarisRASecond = 34;

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
int RAStepsPerDegree = (RACircumference / (RAPulleyTeeth * 2.0) * RAStepsPerRevolution / 360.0);
int DECStepsPerDegree = (DEC_Circumference / (DecPulleyTeeth * 2.0) * DECStepsPerRevolution / 360.0);
