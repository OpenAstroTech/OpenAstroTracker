/*
  =======================================================================================================================================

    Version 1.7

    1. Connect your Arduino, under tools choose "Arduino Uno", set the right Port and set "Arduino ISP" as the Programmer.
    2. Hit upload (Ctrl-U)

    You might need to download the "AccelStepper" library by Mike McCauley

    Authors: /u/intercipere
             /u/clutchplate
             /u/EorEquis

  =======================================================================================================================================
*/
#pragma once

#include "Globals.hpp"

String version = "V1.7.19";

///////////////////////////////////////////////////////////////////////////
// Please see the Globals.h file for configuration of the firmware.
///////////////////////////////////////////////////////////////////////////

// See NORTHERN_HEMISPHERE in Globals.hpp if you NOT in the northern hemisphere


///////////////////////////////////////////////////////////////////////////
// RA stepper                                                            //
///////////////////////////////////////////////////////////////////////////
// This is how many steps your stepper needs for a full rotation.
float RAStepsPerRevolution = 4096;   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200

// The radius of the surface that the belt runs on (in V1 of the ring) was 168.24mm.
// Belt moves 40mm for one stepper revolution (2mm pitch, 20 teeth).
// RA wheel is 2 x PI x 168.24mm (V2:180mm) circumference = 1057.1mm (V2:1131mm)
// One RA revolution needs 26.43 (1057.1mm / 40mm) stepper revolutions (V2: 28.27 (1131mm/40mm))
// Which means 108245 steps (26.43 x 4096) moves 360 degrees (V2: 115812 steps (28.27 x 4096))
// So there are 300.1 steps/degree (108245 / 360)  (V2: 322 (115812 / 360))
// Theoretically correct RA tracking speed is 1.246586 (300 x 14.95903 / 3600) (V2 : 1.333800 (322 x 14.95903 / 3600) steps/sec (this is for 20T)

// Your drive pulley tooth count:
#define RAPulleyTeeth 16

// the Circumference of the RA wheel
// V1: 1057.1
// V2: 1131
#define RACircumference 1131

int RAStepsPerDegree = (RACircumference / (RAPulleyTeeth * 2.0) * RAStepsPerRevolution / 360.0);      // V2 Ring has belt in a groove and belt runs on bearings
int RAspeed = 500;          // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
int RAacceleration = 600;   // High speeds tend to make these cheap steppers unprecice



///////////////////////////////////////////////////////////////////////////
// DEC stepper                                                           //
///////////////////////////////////////////////////////////////////////////
// This is how many steps your stepper needs for a full rotation.
float DECStepsPerRevolution = 4096;   // 28BYJ-48 = 4096  |  NEMA 0.9° = 400  |  NEMA 1.8° = 200

// Belt moves 40mm for one stepper revolution (2mm pitch, 20 teeth).
// DEC wheel is 2 x PI x 90mm circumference which is 565.5mm
// One DEC revolution needs 14.13 (565.5mm/40mm) stepper revolutions
// Which means 57907 steps (14.14 x 4096) moves 360 degrees
// So there are 160.85 steps/degree (57907/360) (this is for 20T)
#define DecPulleyTeeth 16

int DECStepsPerDegree = (568.5 / (DecPulleyTeeth * 2.0) * DECStepsPerRevolution / 360.0);
int DECspeed = 500;           // You can change the speed and acceleration of the steppers here. Max. Speed = 600. 
int DECacceleration = 600;    // High speeds tend to make these cheap steppers unprecice

// These values are needed to calculate the current position during initial alignment.
// Use something like Stellarium to look up the RA of Polaris in JNow (on date) variant.
// This changes slightly over weeks, so adjust every couple of months.
// This value is from 15.Jun.2020, next adjustment suggested at end 2020
// The same could be done for the DEC coordinates but they dont change significantly for the next 5 years
byte PolarisRAHour = 2;
byte PolarisRAMinute = 58;
byte PolarisRASecond = 15;
