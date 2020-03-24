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

#define motorPin1  2    // IN1 auf ULN2003 driver 1    //  2 / 22
#define motorPin2  3    // IN2 auf ULN2003 driver 1    //  3 / 24
#define motorPin3  11    // IN3 auf ULN2003 driver 1   // 11 / 26
#define motorPin4  12    // IN4 auf ULN2003 driver 1   // 12 / 28

#define motorPin11  15    // IN1 auf ULN2003 driver 2
#define motorPin12  16    // IN2 auf ULN2003 driver 2
#define motorPin13  17    // IN3 auf ULN2003 driver 2
#define motorPin14  18    // IN4 auf ULN2003 driver 2

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define RA_Menu 0
#define DEC_Menu 1
#define HA_Menu 2
#define Polaris_Menu 3
#define Heat_Menu 4
#define Calibration_Menu 5
#define Control_Menu 6

int lcd_key     = 0;
int adc_key_in  = 0;
int read_LCD_buttons() {
  adc_key_in = analogRead(0);
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 450)  return btnDOWN;
  if (adc_key_in < 600)  return btnLEFT;
  if (adc_key_in < 920)  return btnSELECT;
  //return btnNONE;
}

// Adjust the given number by the given adjustment, wrap around the limits. 
// Limits are inclusive, so they represent the lowest and highest valid number.
int adjustWrap(int current, int adjustBy, int minVal, int maxVal)
{
  current += adjustBy;
  if (current > maxVal) current -= (maxVal + 1);
  if (current < minVal) current += (maxVal + 1);
  return current;
}

// Adjust the given number by the given adjustment, clamping to the limits. 
// Limits are inclusive, so they represent the lowest and highest valid number.
int adjustClamp(int current, int adjustBy, int minVal, int maxVal)
{
  current += adjustBy;
  if (current > maxVal) current = maxVal;
  if (current < minVal) current = minVal;
  return current;
}

int adjustSecond(int current, int adjustBy) {
  return adjustWrap(current, adjustBy, 0, 59);
}

int adjustMinute(int current, int adjustBy) {
  return adjustWrap(current, adjustBy, 0, 59);
}

int adjustHour(int current, int adjustBy) {
  return adjustWrap(current, adjustBy, 0, 23);
}

String inString = "";
int controlDisplay =0;

AccelStepper stepperRA(FULLSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepperDEC(HALFSTEP, motorPin11, motorPin13, motorPin12, motorPin14);
AccelStepper stepperTRK(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);     //yes, this is the same motor as stepperRA, dont ask why
AccelStepper stepperGUIDE(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

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

//RA stuff
int hourRA=0;
int minRA = 0;
int secRA = 0;
float moveRA;
int RAselect;
int hourRAprint;
int minRAprint;
int secRAprint;

//DEC stuff
int degreeDEC;
int minDEC;
int secDEC;
float moveDEC;
int DECselect;
int printdegDEC;

//Hour correction
int hourHA = 0;
int minHA = 0;
int HAselect;
int hHAcorrection;
int mHAcorrection = -5 ;  //wenn minus dann hHAcorrection -1
int sHAcorrection;
int hourHAzeit;
int minHAzeit;

int heatselect;
int RAheat = 0;
int DECheat = 1;

//Stellarium
char current_RA[9];
char current_DEC[10];
int HAh_save;
int HAm_save;
float slew_RA;
float slew_DEC;

int rightArrow = 4;
int leftArrow = 3;

byte DEG[8] = {
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
  B00000
};

byte min[] = {
  B10000,
  B10000,
  B10000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte sec[] = {
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
