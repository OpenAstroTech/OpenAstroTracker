/*
  =======================================================================================================================================

    Version 1.1

    1. Connect your Arduino, under tools choose "Arduino Uno", set the right Port and set "Arduino ISP" as the Programmer.
    2. Hit upload

    You might need to download the "AccelStepper" library by Mike McCauley

  =======================================================================================================================================
*/

float speed = 1.000; //Use this value to slightly increase or decrese speed. The values from the "CAL" menu will be added to this.

float RevSteps = 4096;
// This is how many steps your 28BYJ-48 stepper needs for a full rotation. It is almost always 4096.
// This code drives the steppers in halfstep mode

// ======================================================================================================================================

int RAspeed = 200;          // You can change the speed and acceleration of the steppers here. Max. Speed = 600. High speeds tend to make
int RAacceleration = 1000;  // these cheap steppers unprecice 
int DECspeed = 200;

// These values are needed to calculate the current position during initial alignment.
int h = 21;   
int m = 2;    
int s = 25;   
// You get these values by calculating 24h minus the current (not J2000) RA of Polaris.
// This changes slightly over weeks, so adjust every couple of months.
// This value is from 27.Nov.19, I suggest next adjustment in mid 2020
// The same could be done for the DEC coordinates but they dont change significantly for the next 5 years
