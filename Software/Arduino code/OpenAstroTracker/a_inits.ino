//#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <AccelStepper.h>
#include <LiquidCrystal.h>

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

#include "Utility.h"
#include "DayTime.hpp"
#include "Mount.hpp"
#include "MeadeCommandProcessor.h"


#define HALFSTEP 8
#define FULLSTEP 4

//SoftwareSerial BT(10,11);

#ifdef __AVR_ATmega2560__  // Arduino Mega

#define RAmotorPin1  22    // IN1 auf ULN2003 driver 1    //  2 / 22
#define RAmotorPin3  24    // IN2 auf ULN2003 driver 1    //  3 / 24
#define RAmotorPin2  26    // IN3 auf ULN2003 driver 1   // 11 / 26
#define RAmotorPin4  28    // IN4 auf ULN2003 driver 1   // 12 / 28

#define DECmotorPin1  30    // IN1 auf ULN2003 driver 2
#define DECmotorPin3  32    // IN2 auf ULN2003 driver 2
#define DECmotorPin2  34    // IN3 auf ULN2003 driver 2
#define DECmotorPin4  36    // IN4 auf ULN2003 driver 2
#endif

#ifdef ESP8266
// RA Motor pins
#define RAmotorPin1  D0    // IN1 auf ULN2003 driver 1    //  2 / 22
#define RAmotorPin3  D1    // IN2 auf ULN2003 driver 1    //  3 / 24d
#define RAmotorPin2  D2    // IN3 auf ULN2003 driver 1   // 11 / 26
#define RAmotorPin4  D3    // IN4 auf ULN2003 driver 1   // 12 / 28

// DEC Motor pins
#define DECmotorPin1  D5    // IN1 auf ULN2003 driver 2
#define DECmotorPin2  D7    // IN2 auf ULN2003 driver 2no
#define DECmotorPin3  D6    // IN3 auf ULN2003 driver 2
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
#define DECmotorPin2  17    // IN2 auf ULN2003 driver 2
#define DECmotorPin3  16    // IN3 auf ULN2003 driver 2
#define DECmotorPin4  18    // IN4 auf ULN2003 driver 2
#endif



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

#ifdef TFTscreen
MCUFRIEND_kbv tft;
const int XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9595
const int TS_LEFT = 897, TS_RT = 174, TS_TOP = 81, TS_BOT = 903;
//const int TS_LEFT = 903, TS_RT = 81, TS_TOP = 897, TS_BOT = 174;
//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define MINPRESSURE 200
#define MAXPRESSURE 1000
#define BLACK 0x0000
#define WHITE 0xFFFF
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define RED 0xF800
#define GREEN 0x07E0
#define GREY 0x7BEF
#define GREYGREEN 0x74AD
#define GREYRED 0x9B2C
Adafruit_GFX_Button coords_btn, go_btn, ctrl_btn, sett_btn, return_btn, test_btn, up_btn, down_btn, right_btn, left_btn, sethome_btn, confirm_btn, decline_btn, slewhome_btn;
Adafruit_GFX_Button one_btn, two_btn, three_btn, four_btn, five_btn, six_btn, seven_btn, eight_btn, nine_btn, zero_btn, enter_btn, erase_btn, slew_btn, RA_btn, DEC_btn;
Adafruit_GFX_Button RAhour_btn, RAmin_btn, RAsec_btn, conformRAcoord_btn;
int pixel_x, pixel_y;
int TFTmenu = 1;
int lastmenu;
int fnct;
int btnpress = 0;
int menuchange;
int tftTargetRAh = 12;  //, tftTargetRAm = 45, tftTargetRAs = 59;
String tftTargetRA = "123456";
bool overlayactive;
bool Touch_getXY(void) {
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);      //restore shared pins
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);   //because TFT control pins
  digitalWrite(XM, HIGH);

  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    //pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); // Portrait
    //pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    pixel_y = map(p.x, TS_RT, TS_LEFT, 0, tft.height()); // Landscape
    pixel_x = map(p.y, TS_TOP, TS_BOT, 0, tft.width());
  }
  return pressed;
}
int background = BLACK;
int boxfill = RED;
int boxborder = WHITE;
int boxtext = BLACK;
//temp stuff
int rahour = 23;
int ramin = 12;
int rasec = 56;


#endif


//debugging
String inBT;
