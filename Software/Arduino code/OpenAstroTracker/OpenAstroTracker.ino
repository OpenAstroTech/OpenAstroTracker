/*
  =======================================================================================================================================

    Version 1.5.10

    1. Connect your Arduino, under tools choose "Arduino Uno", set the right Port and set "Arduino ISP" as the Programmer.
    2. Hit upload (Ctrl-U)

    You might need to download the "AccelStepper" library by Mike McCauley

    Authors: /u/intercipere
             /u/clutchplate
             /u/EorEquis

  =======================================================================================================================================
*/
String version = "V1.5.22";

boolean north = true;    // change this to 'false' if youre in the southern hemisphere

float speed = 1.000;    // Use this value to slightly increase or decrese tracking speed. The values from the "CAL" menu will be added to this.

// The radius of the surface that the belt runs on (in V1 of the ring) was 168.24mm.
// Belt moves 40mm for one stepper revolution (2mm pitch, 20 teeth).
// RA wheel is 2 x PI x 168.24mm circumference = 1057.1mm
// One RA revolution needs 26.43 (1057.1mm / 40mm) stepper revolutions
// Which means 108245 steps (26.43 x 4096) moves 360 degrees
// So there are 300.1 steps/degree (108245 / 360)
int RAStepsPerDegree = 300;      // adjust this value to calibrate RA movement

// Belt moves 40mm for one stepper revolution (2mm pitch, 20 teeth).
// DEC wheel is 2 x PI x 90mm circumference which is 565.5mm
// One DEC revolution needs 14.13 (565.5mm/40mm) stepper revolutions
// Which means 57907 steps (14.14 x 4096) moves 360 degrees
// So there are 160.85 steps/degree (57907/360)
int DECStepsPerDegree = 161;     // Number of steps needed to move DEC motor 1 degree.

// This is how many steps your 28BYJ-48 stepper needs for a full rotation. It is almost always 4096.
// This code drives the steppers in halfstep mode for TRK and DEC, and full step for RA
float StepsPerRevolution = 4096;

int RAspeed = 400;          // You can change the speed and acceleration of the steppers here. Max. Speed = 600. High speeds tend to make
int RAacceleration = 600;   // these cheap steppers unprecice
int DECspeed = 800;
int DECacceleration = 400;

// Define some stepper limits to prevent physical damage to the tracker. This assumes that the home
// point (zero point) has been correctly set to be pointing at the celestial pole.
float RAStepperLimit = 15500;         // Going much more than this each direction will make the ring fall off the bearings.

// These are for 47N, so they will need adjustment if you're a lot away from that.
// You can use the CTRL menu to find the limits and place them here. I moved it
// down until my lens was horizontal. Note the DEC number. Then move it up until
// the lens is horizontal and note that number. Put those here. Always watch your
// tracker and hit RESET if it approaches a dangerous area.
float DECStepperDownLimit = 10000;    // Going much more than this will make the lens collide with the ring
float DECStepperUpLimit = -22000;     // Going much more than this is going below the horizon.

// These values are needed to calculate the current position during initial alignment.
int h = 21;
int m = 2;
int s = 25;
// You get these values by calculating 24h minus the current (not J2000) RA of Polaris.
// This changes slightly over weeks, so adjust every couple of months.
// This value is from 27.Nov.19, I suggest next adjustment in mid 2020
// The same could be done for the DEC coordinates but they dont change significantly for the next 5 years

// Comment this out to save some code space
// #define DEBUG_MODE

// Uncomment this to support the heating menu
// #define SUPPORT_HEATING

// Uncomment to run a key diagnostic
// #define LCD_BUTTON_TEST
