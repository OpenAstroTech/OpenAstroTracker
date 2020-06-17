// Create the LCD menu variable and initialize the LCD (16x2 characters)

#pragma once

LcdMenu lcdMenu(16, 2, MAXMENUITEMS);
LcdButtons lcdButtons(0);

#ifdef ESP32
DRAM_ATTR Mount mount(RAStepsPerDegree, DECStepsPerDegree, &lcdMenu);
#else
Mount mount(RAStepsPerDegree, DECStepsPerDegree, &lcdMenu);
#endif

#ifdef WIFI_ENABLED
#include "WifiControl.hpp"
WifiControl wifiControl(&mount, &lcdMenu);
#endif

void finishSetup();

/////////////////////////////////
//   ESP32
/////////////////////////////////
#if defined(ESP32) && !defined(RUN_STEPPERS_IN_MAIN_LOOP)
// Forward declare the two functions we run in the main loop
void serialLoop();
void finishSetup();

TaskHandle_t StepperTask;
TaskHandle_t  CommunicationsTask;

/////////////////////////////////
//
// stepperControlFunc
//
// This task function is run on Core 1 of the ESP32
/////////////////////////////////
void IRAM_ATTR stepperControlTask(void* payload)
{
  Mount* mount = reinterpret_cast<Mount*>(payload);
  for (;;) {
    mount->interruptLoop();
    vTaskDelay(1);
  }
}

/////////////////////////////////
//
// mainLoopFunc
//
// This task function is run on Core 2 of the ESP32
/////////////////////////////////
void IRAM_ATTR mainLoopTask(void* payload)
{
  finishSetup();

  for (;;) {
    serialLoop();
    vTaskDelay(1);
  }
}
#endif

/////////////////////////////////
//
// Main program setup 
//
/////////////////////////////////
void setup() {

// Microstepping ---------------
#if RA_Stepper_TYPE == 1   // RA driver startup (for TMC2209)
  pinMode(41, OUTPUT);
  //digitalWrite(43, HIGH);  // SPREAD
  digitalWrite(41, LOW);  // ENABLE

  digitalWrite(42, LOW);  // MS2
  digitalWrite(40, HIGH);  // MS1
#endif
#if DEC_Stepper_TYPE == 1  // DEC driver startup (for TMC2209)
  pinMode(45, OUTPUT);
  digitalWrite(45, LOW);  // ENABLE

  digitalWrite(46, LOW);  // MS2
  digitalWrite(44, HIGH);  // MS1
#endif

  //debug_init();
  Serial.begin(57600);
  //BT.begin(9600);

#ifdef DEBUG_MODE
  Serial.println("Hello, universe!");
#endif

/////////////////////////////////
// ESP32
/////////////////////////////////
#if defined(ESP32) && !defined(RUN_STEPPERS_IN_MAIN_LOOP)
  disableCore0WDT();
  xTaskCreatePinnedToCore(
    stepperControlTask,    // Function to run on this core
    "StepperControl",      // Name of this task
    32767,                 // Stack space in bytes
    &mount,                // payload
    2,                     // Priority (2 is higher than 1)
    &StepperTask,          // The location that receives the thread id
    0);                    // The core to run this on

  delay(100);

  xTaskCreatePinnedToCore(
    mainLoopTask,             // Function to run on this core
    "CommunicationControl",   // Name of this task
    32767,                    // Stack space in bytes
    NULL,                     // payload
    1,                        // Priority (2 is higher than 1)
    &CommunicationsTask,      // The location that receives the thread id
    1);                       // The core to run this on

  delay(100);

#else

  finishSetup();

#endif
}

void finishSetup()
{
  // Show a splash screen
  lcdMenu.setCursor(0, 0);
  lcdMenu.printMenu("OpenAstroTracker");
  lcdMenu.setCursor(0, 1);
  lcdMenu.printMenu("     " + version);
  unsigned long now = millis();

  // Create the command processor singleton
  MeadeCommandProcessor::createProcessor(&mount, &lcdMenu);

#ifdef WIFI_ENABLED
  wifiControl.setup();
#endif

  // Configure the mount
  
  // Set the stepper motor parameters
  // Set the stepper motor parameters
  #if RA_Stepper_TYPE == 0 && DEC_Stepper_TYPE == 0
    mount.configureRAStepper(FULLSTEP, RAmotorPin1, RAmotorPin2, RAmotorPin3, RAmotorPin4, RAspeed, RAacceleration);
    mount.configureDECStepper(HALFSTEP, DECmotorPin1, DECmotorPin2, DECmotorPin3, DECmotorPin4, DECspeed, DECacceleration);
  #endif
  #if RA_Stepper_TYPE == 1 && DEC_Stepper_TYPE == 0
    mount.configureRAStepper(DRIVER, RAmotorPin1, RAmotorPin2, RAspeed, RAacceleration);
    mount.configureDECStepper(HALFSTEP, DECmotorPin1, DECmotorPin2, DECmotorPin3, DECmotorPin4, DECspeed, DECacceleration);
  #endif
  #if RA_Stepper_TYPE == 1 && DEC_Stepper_TYPE == 1
    mount.configureRAStepper(DRIVER, RAmotorPin1, RAmotorPin2, RAspeed, RAacceleration);
    mount.configureDECStepper(DRIVER, DECmotorPin1, DECmotorPin2, DECspeed, DECacceleration);
  #endif

  // The mount uses EEPROM storage locations 0-10 that it reads during construction

  // Read other persisted values and set in mount
  DayTime haTime = DayTime(EEPROM.read(1), EEPROM.read(2), 0);

#ifdef DEBUG_MODE
  Serial.println("SpeedCal: " + String(mount.getSpeedCalibration(), 5));
  Serial.println("TRKSpeed: " + String(mount.getSpeed(TRACKING), 5));
#endif

  mount.setHA(haTime);

  // For LCD screen, it's better to initialize the target to where we are (RA)
  mount.targetRA() = mount.currentRA();

  // Hook into the timers for periodic interrupts to run the steppers. 
  mount.startTimerInterrupts();

  // Start the tracker.
  mount.startSlewing(TRACKING);

#ifndef HEADLESS_CLIENT
  // Create the LCD top-level menu items
  lcdMenu.addItem("RA", RA_Menu);
  lcdMenu.addItem("DEC", DEC_Menu);

#ifdef SUPPORT_POINTS_OF_INTEREST
  lcdMenu.addItem("GO", POI_Menu);
#else
  lcdMenu.addItem("GO", Home_Menu);
#endif

  lcdMenu.addItem("HA", HA_Menu);

#ifdef SUPPORT_HEATING
  lcdMenu.addItem("HEA", Heat_Menu);
#endif

#ifdef SUPPORT_MANUAL_CONTROL
  lcdMenu.addItem("CTRL", Control_Menu);
#endif

#ifdef SUPPORT_CALIBRATION
  lcdMenu.addItem("CAL", Calibration_Menu);
#endif

#ifdef SUPPORT_INFO_DISPLAY
  lcdMenu.addItem("INFO", Status_Menu);
#endif

  while (millis() - now < 750) {
    mount.loop();
  }

  lcdMenu.updateDisplay();
#endif // HEADLESS_CLIENT

#ifdef DEBUG_MODE
  Serial.println("Setup done!");
#endif


}
