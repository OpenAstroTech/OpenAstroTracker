// If you really want to look through this code, i apologise for my terrible coding
//#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <AccelStepper.h>
#include <LiquidCrystal.h>
//#include <SoftwareSerial.h>

#define HALFSTEP 8
#define FULLSTEP 4

//SoftwareSerial BT(10,11);

// RA Motor pins
#define motorPin1  2    // IN1 auf ULN2003 driver 1    //  2 / 22
#define motorPin2  3    // IN2 auf ULN2003 driver 1    //  3 / 24
#define motorPin3  13    // IN3 auf ULN2003 driver 1   // 11 / 26
#define motorPin4  11    // IN4 auf ULN2003 driver 1   // 12 / 28

// DEC Motor pins
#define motorPin11  15    // IN1 auf ULN2003 driver 2
#define motorPin12  16    // IN2 auf ULN2003 driver 2
#define motorPin13  17    // IN3 auf ULN2003 driver 2
#define motorPin14  18    // IN4 auf ULN2003 driver 2

// LCD shield buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// Menu IDs
#define RA_Menu 0
#define DEC_Menu 1
#define HA_Menu 2
#define Heat_Menu 4
#define Calibration_Menu 5
#define Control_Menu 6
#define Home_Menu 7
#define POI_Menu 8
#define Status_Menu 9

// How many menu items at most?
#define MAXMENUITEMS 10

#define LESS_THAN 0x3C
#define GREATER_THAN 0x3E

// Stepper control for RA and DEV.
// TRK is used for live tracking and runs in parallel with RA.
// GUIDE is used for Stellarium control
AccelStepper stepperRA(FULLSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepperDEC(HALFSTEP, motorPin11, motorPin13, motorPin12, motorPin14);

// Use another AccelStepper to run the RA motor as well. This instance tracks earths rotation.
AccelStepper stepperTRK(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

String inString = "";

bool inStartup = true;        // Start with a guided startup

// Serial control variables
bool inSerialControl = false; // When the serial port is in control
bool serialIsSlewing = false; // When the serial port is slewing the tracker
bool slewingToHome = false;   // When the serial port is slewing to home/park.
bool quitSerialOnNextButtonRelease = false; // Used to detect SELECT button to quit Serial mode.
String logString;
boolean isPulseGuiding = true;

// Display related variables
#define DISPLAY_UPDATE_TIME 150 // Every how many milliseconds to update the RA and DEC coordinates while (slewing)
long lastDisplayUpdate = 0;  // The time when we last updated RA and DEC coordinates (while slewing)
float totalDECMove = 0;      // The number of DEC steps we asked for
float totalRAMove = 0;

// Calibration variables
float inputcal;              // calibration variable set form as integer. Added to speed after dividing by 10000
float speedCalibration;      // speed calibration factor
int calDelay = 150;          // The current delay in ms when changing calibration value. The longer a button is depressed, the smaller this gets.

// Variables for use in the CONTROL menu
bool inControlMode  = false;  // Is manual control enabled
byte StateMaskDEC   = B0011;   // Is the DEC stepper running?
byte StateMaskUp    = B0001;    // Is the DEC stepper moving upwards?
byte StateMaskDown  = B0010;  // Is the DEC stepper moving downwards?
byte StateMaskRA    = B1100;    // Is the RA stepper running?
byte StateMaskLeft  = B0100;  // Is the RA stepper moving left (CW)?
byte StateMaskRight = B1000; // Is the RA stepper moving right (CCW)?
byte controlState   = B0000;   // The current state of the steppers (combination of above values)

// Main loop variables
int lcd_key     = 0;         // The current key state
int adc_key_in  = 0;         // The analog value of the keys
bool waitForButtonRelease = false; // When a button is pressed should we wait for its release before another loop?
int lastKey = btnNONE;       // The key state when we last came through the loop

// Global variables
bool isUnreachable = false;
char scratchBuffer[32];

unsigned long Zeit;

// Stepper motor variables
float stepsPerHour;           // How many steps does the RA stepper need to make to move 15 degrees (1 hour of earths rotation)
float trackingSpeed;          // At what speed does the RA stepper need to run to match earths rotation (steps/sec)
int tracking = 1;             // Are we tracking earths rotation?

// PC Control variables.
boolean pcControl = false;

int direction_old = 1;
int direction_new;


// RA variables
float moveRA;
int RAselect;

// DEC variables
int degreeDEC;
int minDEC;
int secDEC;
float moveDEC;
int DECselect;
int printdegDEC;

// HA variables
int HAselect;
unsigned long lastHAset = 0;

// HEAT menu variables
int heatselect;   // Which stepper are we changing?
int RAheat = 0;   // Are we heating the RA stepper?
int DECheat = 0;  // Are we heating the DEC stepper?

// Stellarium variables
int HAh_save;
int HAm_save;
float slew_RA;
float slew_DEC;

//debugging
String inBT;
