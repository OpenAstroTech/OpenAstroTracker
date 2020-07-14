// Create the LCD menu variable and initialize the LCD (16x2 characters)

#pragma once

#include "Configuration.hpp"
#include "a_inits.hpp"
#include "LcdMenu.hpp"
#include "Utility.hpp"
#include "EPROMStore.hpp"

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
#if defined(ESP32) && (RUN_STEPPERS_IN_MAIN_LOOP == 0)
// Forward declare the two functions we run in the main loop
void serialLoop();

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

/////////////////////////////////
//   Microstepping/driver pins
/////////////////////////////////
#if RA_Stepper_TYPE == 1   // RA driver startup (for A4988)
#if RA_Driver_TYPE == 1
  // include A4988 microstep pins
  //#error "Define Microstep pins and delete this error."
  digitalWrite(40, HIGH);  // MS0
  digitalWrite(41, HIGH);  // MS1
  digitalWrite(42, HIGH);  // MS2
  #endif
#if RA_Driver_TYPE == 2
  // include TMC2209 Standalone pins
  pinMode(40, OUTPUT);
  digitalWrite(40, LOW);  // ENABLE, LOW to enable
  digitalWrite(41, HIGH);  // MS2
  digitalWrite(42, HIGH);  // MS1
  #endif
#if RA_Driver_TYPE == 3
  // include TMC2209 UART pins  
  pinMode(RA_EN_PIN, OUTPUT);
  pinMode(RA_DIAG_PIN, INPUT);
  digitalWrite(RA_EN_PIN, LOW);  // Logic LOW to enable driver
  RA_SERIAL_PORT.begin(57600);  // Start HardwareSerial comms with driver
#endif
#endif
#if DEC_DRIVER_TYPE == 1  // DEC driver startup (for A4988)
  pinMode(45, OUTPUT);
  digitalWrite(45, LOW);  // ENABLE
  digitalWrite(46, LOW);  // MS2
  digitalWrite(44, HIGH);  // MS1
#endif
#if DEC_Driver_TYPE == 2
  // include TMC2209 Standalone pins  TODO-----------------------
  //pinMode(40, OUTPUT);
  //digitalWrite(40, LOW);  // ENABLE, LOW to enable
  //digitalWrite(41, HIGH);  // MS2
  //digitalWrite(42, HIGH);  // MS1
#endif
#if DEC_Driver_TYPE == 3
  // include TMC2209 UART pins  
  pinMode(DEC_EN_PIN, OUTPUT);
  pinMode(DEC_DIAG_PIN, INPUT);
  //pinMode(DEC_MS1_PIN, OUTPUT);
  digitalWrite(DEC_EN_PIN, LOW);  // Logic LOW to enable driver
  //digitalWrite(DEC_MS1_PIN, HIGH); // Logic HIGH to MS1 to get 0b01 address
  DEC_SERIAL_PORT.begin(57600);  // Start HardwareSerial comms with driver
#endif
// end microstepping -------------------

  Serial.begin(57600);
  //BT.begin(9600);

  LOGV2(DEBUG_ANY, "Hello, universe, this is OAT %s!", version.c_str());

  EPROMStore::initialize();

/////////////////////////////////
// ESP32
/////////////////////////////////
#if defined(ESP32) && (RUN_STEPPERS_IN_MAIN_LOOP == 0)
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
  LOGV1(DEBUG_ANY, "Finishing setup...");
  // Show a splash screen
  lcdMenu.setCursor(0, 0);
  lcdMenu.printMenu("OpenAstroTracker");
  lcdMenu.setCursor(0, 1);
  lcdMenu.printMenu("     " + version);
#if HEADLESS_CLIENT == 0
  unsigned long now = millis();
#endif
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

  #if RA_Driver_TYPE == 3
    mount.configureRAdriver(&RA_SERIAL_PORT, R_SENSE, RA_DRIVER_ADDRESS, RA_RMSCURRENT, RA_STALL_VALUE);
  #endif
  #if DEC_Driver_TYPE == 3
    mount.configureDECdriver(&DEC_SERIAL_PORT, R_SENSE, DEC_DRIVER_ADDRESS, DEC_RMSCURRENT, DEC_STALL_VALUE);
  #endif

  // The mount uses EEPROM storage locations 0-10 that it reads during construction
  // The LCD uses EEPROM storage location 11
  mount.readConfiguration();
  
  // Read other persisted values and set in mount
  DayTime haTime = DayTime(EPROMStore::Storage()->read(1), EPROMStore::Storage()->read(2), 0);

  LOGV2(DEBUG_INFO, "SpeedCal: %s", String(mount.getSpeedCalibration(), 5).c_str());
  LOGV2(DEBUG_INFO, "TRKSpeed: %s", String(mount.getSpeed(TRACKING), 5).c_str());

  mount.setHA(haTime);

  // For LCD screen, it's better to initialize the target to where we are (RA)
  mount.targetRA() = mount.currentRA();

  // Hook into the timers for periodic interrupts to run the steppers. 
  mount.startTimerInterrupts();

  // Start the tracker.
  mount.startSlewing(TRACKING);

#if HEADLESS_CLIENT == 0
  // Create the LCD top-level menu items
  lcdMenu.addItem("RA", RA_Menu);
  lcdMenu.addItem("DEC", DEC_Menu);

#if SUPPORT_POINTS_OF_INTEREST == 1
  lcdMenu.addItem("GO", POI_Menu);
#else
  lcdMenu.addItem("GO", Home_Menu);
#endif

  lcdMenu.addItem("HA", HA_Menu);

#if SUPPORT_HEATING == 1
  lcdMenu.addItem("HEA", Heat_Menu);
#endif

#if SUPPORT_MANUAL_CONTROL == 1
  lcdMenu.addItem("CTRL", Control_Menu);
#endif

#if SUPPORT_CALIBRATION == 1
  lcdMenu.addItem("CAL", Calibration_Menu);
#endif

#if SUPPORT_INFO_DISPLAY == 1
  lcdMenu.addItem("INFO", Status_Menu);
#endif

  while (millis() - now < 750) {
    mount.loop();
  }

  lcdMenu.updateDisplay();
#endif // HEADLESS_CLIENT

  LOGV1(DEBUG_ANY, "Setup done!");
}
