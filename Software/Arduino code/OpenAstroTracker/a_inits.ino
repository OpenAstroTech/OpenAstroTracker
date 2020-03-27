// If you really want to look through this code, i apologise for my terrible coding
//#include <SoftwareSerial.h>
#include <stdarg.h>
#include <EEPROM.h>
#include <AccelStepper.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#define HALFSTEP 8
#define FULLSTEP 4

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
//SoftwareSerial BT(10,11);

// RA Motor pins
#define motorPin1  2    // IN1 auf ULN2003 driver 1    //  2 / 22
#define motorPin2  3    // IN2 auf ULN2003 driver 1    //  3 / 24
#define motorPin3  11    // IN3 auf ULN2003 driver 1   // 11 / 26
#define motorPin4  12    // IN4 auf ULN2003 driver 1   // 12 / 28

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
#define Polaris_Menu 3
#define Heat_Menu 4
#define Calibration_Menu 5
#define Control_Menu 6
#define Home_Menu 7
#define POI_Menu 8

// Stepper control for RA and DEV. 
// TRK is used for live tracking and runs in parallel with RA.
// GUIDE is used for Stellarium control
AccelStepper stepperRA(FULLSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepperDEC(HALFSTEP, motorPin11, motorPin13, motorPin12, motorPin14);
AccelStepper stepperTRK(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);     //yes, this is the same motor as stepperRA, dont ask why
AccelStepper stepperGUIDE(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

int lcd_key     = 0;         // The current key state 
int adc_key_in  = 0;         // The analog value of the keys

String inString = "";

bool inStartup = true;       // Start with a guided startup

int calDelay = 150;          // The current delay when changing calibration value. The longer a button is depressed, the samller this gets.

bool waitForButtonRelease = false; // When a button is pressed should we wait for its release before another loop?

// Variables for use in the CONTROL menu
int StateMaskDEC = 0x0011;   // Is the DEC stepper running?
int StateMaskUp = 0x0001;    // Is the DEC stepper moving upwards?
int StateMaskDown = 0x0010;  // Is the DEC stepper moving downwards?
int StateMaskRA = 0x1100;    // Is the RA stepper running?
int StateMaskLeft = 0x0100;  // Is the RA stepper moving left (CW)?
int StateMaskRight = 0x1000; // Is the RA stepper moving right (CCW)?

int controlState = 0x0000;   // The current state of the steppers (combination of above values)
int controlDisplay = 0;      // How many loop cycles left before updating the display
bool inControlMode = false;  // Is manual control enabled

int lastKey = btnNONE;       // The key state when we last came through the loop

float totalDECMove = 0;      // The number of DEC steps we asked for
float totalRAMove = 0;
bool isUnreachable = false;

//String inCmd;
//String inputString = "";
//String commandString = "";
//boolean isConnected = false;

String logString;
//boolean stringComplete = false;
//unsigned long timeWait;
boolean isPulseGuiding = true;

unsigned long Zeit;
float stepsPerHour;

boolean pcControl = false;
int currentSecs;
int currentMins;
float inputcal;
float speedcalibration;
int direction_old = 1;
int direction_new;

int hPolarisPosition;
int mPolarisPosition;

int tracking = 1;
float trackingspeed;

// RA stuff
float moveRA;
int RAselect;

//DEC stuff
int degreeDEC;
int minDEC;
int secDEC;
float moveDEC;
int DECselect;
int printdegDEC;

//Hour stuff
int HAselect;

// HEAT menu settings
int heatselect;   // Which stepper are we changing?
int RAheat = 0;   // Are we heating the RA stepper?
int DECheat = 0;  // Are we heating the DEC stepper?

//Stellarium
char current_RA[9];
char current_DEC[10];
int HAh_save;
int HAm_save;
float slew_RA;
float slew_DEC;

// LCD special chars
int leftArrow = 3;
int rightArrow = 4;

byte DegreesBitmap[8] = {
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
  B00000
};

byte MinutesBitmap[] = {
  B10000,
  B10000,
  B10000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte SecondsBitmap[] = {
  B10100,
  B10100,
  B10100,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

//debugging
String inBT;
