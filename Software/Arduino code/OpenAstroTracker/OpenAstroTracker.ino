/*
  =======================================================================================================================================

    Version 1.6

    1. Connect your Arduino, under tools choose "Arduino Uno", set the right Port and set "Arduino ISP" as the Programmer.
    2. Hit upload (Ctrl-U)

    You might need to download the "AccelStepper" library by Mike McCauley

    Authors: /u/intercipere
             /u/clutchplate
             /u/EorEquis

  =======================================================================================================================================
*/
#include "Globals.h"

String version = "V1.6.32";

///////////////////////////////////////////////////////////////////////////
// Please see the Globals.h file for configuration of the firmware.
///////////////////////////////////////////////////////////////////////////

// See NORTHERN_HEMISPHERE in Globals.h if you not in the northern hemisphere

// The radius of the surface that the belt runs on (in V1 of the ring) was 168.24mm.
// Belt moves 40mm for one stepper revolution (2mm pitch, 20 teeth).
// RA wheel is 2 x PI x 168.24mm (V2:180mm) circumference = 1057.1mm (V2:1131mm)
// One RA revolution needs 26.43 (1057.1mm / 40mm) stepper revolutions (V2: 28.27 (1131mm/40mm))
// Which means 108245 steps (26.43 x 4096) moves 360 degrees (V2: 115812 steps (28.27 x 4096))
// So there are 300.1 steps/degree (108245 / 360)  (V2: 322 (115812 / 360))
// Theoretically correct RA tracking speed is 1.246586 (300 x 14.95903 / 3600) (V2 : 1.333800 (322 x 14.95903 / 3600) steps/sec
#error "Please uncomment one of the two following lines depending on which version of the RA ring you printed. And comment out this line."
// int RAStepsPerDegree = 300;      // V1 Ring has a ridge on top of the ring that the belt runs on and the ring runs on the bearings
// int RAStepsPerDegree = 322;      // V2 Ring has belt in a groove and belt runs on bearings


// Belt moves 40mm for one stepper revolution (2mm pitch, 20 teeth).
// DEC wheel is 2 x PI x 90mm circumference which is 565.5mm
// One DEC revolution needs 14.13 (565.5mm/40mm) stepper revolutions
// Which means 57907 steps (14.14 x 4096) moves 360 degrees
// So there are 160.85 steps/degree (57907/360)
int DECStepsPerDegree = 161;     // Number of steps needed to move DEC motor 1 degree.

// This is how many steps your 28BYJ-48 stepper needs for a full rotation. It is almost always 4096.
// This code drives the steppers in halfstep mode for TRK and DEC, and full step for RA
float StepsPerRevolution = 4096;

float speed = 1.000;    // Use this value to slightly increase or decrese tracking speed. The values from the "CAL" menu will be added to this.

int RAspeed = 400;          // You can change the speed and acceleration of the steppers here. Max. Speed = 600. High speeds tend to make
int RAacceleration = 600;   // these cheap steppers unprecice
int DECspeed = 800;
int DECacceleration = 400;

// Define some stepper limits to prevent physical damage to the tracker. This assumes that the home
// point (zero point) has been correctly set to be pointing at the celestial pole.
// Note: these are currently not used
float RAStepperLimit = 15500;         // Going much more than this each direction will make the ring fall off the bearings.

// These are for 47N, so they will need adjustment if you're a lot away from that.
// You can use the CTRL menu to find the limits and place them here. I moved it
// down until my lens was horizontal. Note the DEC number. Then move it up until
// the lens is horizontal and note that number. Put those here. Always watch your
// tracker and hit RESET if it approaches a dangerous area.
// Note: these are currently not used
float DECStepperDownLimit = 10000;    // Going much more than this will make the lens collide with the ring
float DECStepperUpLimit = -22000;     // Going much more than this is going below the horizon.

// These values are needed to calculate the current position during initial alignment.
int PolarisRAHour = 2;
int PolarisRAMinute = 58;
int PolarisRASecond = 0;
// Use something like Stellarium to look up the RA of Polaris in JNow (on date) variant.
// This changes slightly over weeks, so adjust every couple of months.
// This value is from 18.Apr.2020, next adjustment suggested at end 2020
// The same could be done for the DEC coordinates but they dont change significantly for the next 5 years
