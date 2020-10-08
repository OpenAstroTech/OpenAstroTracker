// Create the LCD menu variable and initialize the LCD (16x2 characters)

#pragma once

#include "inc/Config.hpp"
#include "a_inits.hpp"
#include "LcdMenu.hpp"
#include "Utility.hpp"
#include "EPROMStore.hpp"
#include "inc/Config.hpp"

LcdMenu lcdMenu(16, 2, MAXMENUITEMS);
LcdButtons lcdButtons(0);

#ifdef ESP32
DRAM_ATTR Mount mount(RAStepsPerDegree, DECStepsPerDegree, &lcdMenu);
#else
Mount mount(RA_STEPS_PER_DEGREE, DEC_STEPS_PER_DEGREE, &lcdMenu);
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

  #if USE_GPS == 1
  GPS_SERIAL_PORT.begin(GPS_BAUD_RATE);
  #endif

  /////////////////////////////////
  //   Microstepping/driver pins
  /////////////////////////////////
  #if RA_STEPPER_TYPE == STEPPER_TYPE_NEMA17  // RA driver startup (for A4988)
    #if RA_DRIVER_TYPE == DRIVER_TYPE_GENERIC
      // include A4988 microstep pins
      //#error "Define Microstep pins and delete this error."
      digitalWrite(RA_EN_PIN, HIGH);
      digitalWrite(RA_MS0_PIN, HIGH);  // MS0
      digitalWrite(RA_MS1_PIN, HIGH);  // MS1
      digitalWrite(RA_MS2_PIN, HIGH);  // MS2
      #endif
    #if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_STANDALONE
      // include TMC2209 Standalone pins
      pinMode(40, OUTPUT);
      digitalWrite(40, LOW);  // ENABLE, LOW to enable
      digitalWrite(41, HIGH);  // MS2
      digitalWrite(42, HIGH);  // MS1
      #endif
    #if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
      // include TMC2209 UART pins  
      pinMode(RA_EN_PIN, OUTPUT);
      pinMode(RA_DIAG_PIN, INPUT);
      digitalWrite(RA_EN_PIN, LOW);  // Logic LOW to enable driver
      RA_SERIAL_PORT.begin(57600);  // Start HardwareSerial comms with driver
    #endif
  #endif
  #if DEC_STEPPER_TYPE == STEPPER_TYPE_NEMA17  // DEC driver startup (for A4988)
    #if DEC_DRIVER_TYPE == DRIVER_TYPE_GENERIC  // DEC driver startup (for A4988)
      digitalWrite(DEC_EN_PIN, HIGH);
      digitalWrite(DEC_MS0_PIN, HIGH);  // MS1
      digitalWrite(DEC_MS1_PIN, HIGH);  // MS2
      digitalWrite(DEC_MS2_PIN, HIGH);  // MS3
    #endif
    #if DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_STANDALONE
      // include TMC2209 Standalone pins  TODO-----------------------
      //pinMode(40, OUTPUT);
      //digitalWrite(40, LOW);  // ENABLE, LOW to enable
      //digitalWrite(41, HIGH);  // MS2
      //digitalWrite(42, HIGH);  // MS1
    #endif
    #if DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
      // include TMC2209 UART pins  
      pinMode(DEC_EN_PIN, OUTPUT);
      pinMode(DEC_DIAG_PIN, INPUT);
      //pinMode(DEC_MS1_PIN, OUTPUT);
      digitalWrite(DEC_EN_PIN, LOW);  // Logic LOW to enable driver
      //digitalWrite(DEC_MS1_PIN, HIGH); // Logic HIGH to MS1 to get 0b01 address
      DEC_SERIAL_PORT.begin(57600);  // Start HardwareSerial comms with driver
    #endif
  #endif
  // end microstepping -------------------

  Serial.begin(57600);
  //BT.begin(9600);

  LOGV2(DEBUG_ANY, F("Hello, universe, this is OAT %s!"), VERSION);

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
  LOGV1(DEBUG_ANY, F("Finishing setup..."));
  // Show a splash screen
  lcdMenu.setCursor(0, 0);
  lcdMenu.printMenu("OpenAstroTracker");
  lcdMenu.setCursor(5, 1);
  lcdMenu.printMenu(VERSION);

  #if HEADLESS_CLIENT == 0
    // Check for EEPROM reset (Button down during boot)
    if (lcdButtons.currentState() == btnDOWN){
      LOGV1(DEBUG_INFO, F("Erasing configuration in EEPROM!"));
      mount.clearConfiguration();
      // Wait for button release
      lcdMenu.setCursor(13, 1);
      lcdMenu.printMenu("CLR");
      while (lcdButtons.currentState() != btnNONE) {
        delay(10);
      }
    }

    unsigned long now = millis();
  #endif
  // Create the command processor singleton
  LOGV1(DEBUG_ANY, F("Initialize LX200 handler..."));
  MeadeCommandProcessor::createProcessor(&mount, &lcdMenu);

  #ifdef WIFI_ENABLED
    LOGV1(DEBUG_ANY, F("Setup Wifi..."));
    wifiControl.setup();
  #endif

  // Configure the mount
  // Delay for a while to get UARTs booted...
  delay(1000);  

  LOGV1(DEBUG_ANY, F("Configure RA stepper..."));
  // Set the stepper motor parameters
  #if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48 
    mount.configureRAStepper(FULLSTEP_MODE, RAmotorPin1, RAmotorPin2, RAmotorPin3, RAmotorPin4, RA_STEPPER_SPEED, RA_STEPPER_ACCELERATION);
  #elif RA_STEPPER_TYPE == STEPPER_TYPE_NEMA17
    mount.configureRAStepper(DRIVER_MODE, RAmotorPin1, RAmotorPin2, RA_STEPPER_SPEED, RA_STEPPER_ACCELERATION);
  #else
    #error New stepper type? Configure it here.
  #endif

  LOGV1(DEBUG_ANY, F("Configure DEC stepper..."));
  #if DEC_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
    LOGV1(DEBUG_ANY, "Configure DEC stepper 28BYJ-48...");
    mount.configureDECStepper(HALFSTEP_MODE, DECmotorPin1, DECmotorPin2, DECmotorPin3, DECmotorPin4, RA_STEPPER_SPEED, DEC_STEPPER_ACCELERATION);
  #elif DEC_STEPPER_TYPE == STEPPER_TYPE_NEMA17
    LOGV1(DEBUG_ANY, F("Configure DEC stepper NEMA..."));
    mount.configureDECStepper(DRIVER_MODE, DECmotorPin1, DECmotorPin2, RA_STEPPER_SPEED, DEC_STEPPER_ACCELERATION);
  #else
    #error New stepper type? Configure it here.
  #endif

  #if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
    LOGV1(DEBUG_ANY, F("Configure RA driver..."));
    mount.configureRAdriver(&RA_SERIAL_PORT, R_SENSE, RA_DRIVER_ADDRESS, RA_RMSCURRENT, RA_STALL_VALUE);
  #endif
  #if DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
    LOGV1(DEBUG_ANY, F("Configure DEC driver TMC2009 UART..."));
    mount.configureDECdriver(&DEC_SERIAL_PORT, R_SENSE, DEC_DRIVER_ADDRESS, DEC_RMSCURRENT, DEC_STALL_VALUE);
  #endif

  #if AZIMUTH_ALTITUDE_MOTORS == 1
    LOGV1(DEBUG_ANY, F("Configure AZ stepper..."));
    mount.configureAzStepper(HALFSTEP_MODE, AZmotorPin1, AZmotorPin2, AZmotorPin3, AZmotorPin4, AZIMUTH_MAX_SPEED, AZIMUTH_MAX_ACCEL);
    LOGV1(DEBUG_ANY, F("Configure Alt stepper..."));
    mount.configureAltStepper(FULLSTEP_MODE, ALTmotorPin1, ALTmotorPin2, ALTmotorPin3, ALTmotorPin4, ALTITUDE_MAX_SPEED, ALTITUDE_MAX_ACCEL);
  #endif

  // The mount uses EEPROM storage locations 0-10 that it reads during construction
  // The LCD uses EEPROM storage location 11
  mount.readConfiguration();
  
  // Read other persisted values and set in mount
  DayTime haTime = DayTime(EPROMStore::read(1), EPROMStore::read(2), 0);

  LOGV2(DEBUG_INFO, "SpeedCal: %s", String(mount.getSpeedCalibration(), 5).c_str());
  LOGV2(DEBUG_INFO, "TRKSpeed: %s", String(mount.getSpeed(TRACKING), 5).c_str());

  mount.setHA(haTime);

  // For LCD screen, it's better to initialize the target to where we are (RA)
  mount.targetRA() = mount.currentRA();

  // Hook into the timers for periodic interrupts to run the steppers. 
  mount.startTimerInterrupts();

  // Start the tracker.
  LOGV1(DEBUG_ANY, F("Start Tracking..."));
  mount.startSlewing(TRACKING);

  #if HEADLESS_CLIENT == 0
    LOGV1(DEBUG_ANY, F("Setup menu system..."));

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

    LOGV1(DEBUG_ANY, F("Update display..."));
    lcdMenu.updateDisplay();
  #endif // not HEADLESS_CLIENT

  mount.bootComplete();
  LOGV1(DEBUG_ANY, F("Setup done!"));
}
