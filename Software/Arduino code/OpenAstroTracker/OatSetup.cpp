// Create the LCD menu variable and initialize the LCD (16x2 characters)
#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/Button.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "lib/input/LcdButtons.hpp"
#include "lib/util/debug.hpp"
#include "StringTable.hpp"
#include "Configuration.hpp"
#include "Mount.hpp"
#include "MeadeCommandProcessor.hpp"
#include "Utility.hpp"
#include "EPROMStore.hpp"
#include "OatMenu.hpp"
#include "a_inits.hpp"

LcdDisplay lcdDisplay(16, 2);
LcdButtons lcdButtons(0);
MainMenu mainMenu(&lcdDisplay);

#ifdef ESP32
DRAM_ATTR Mount mount(RAStepsPerDegree, DECStepsPerDegree, &lcdDisplay);
#else
Mount mount(RAStepsPerDegree, DECStepsPerDegree, &lcdDisplay);
#endif

#ifdef WIFI_ENABLED
#include "WifiControl.hpp"
WifiControl wifiControl(&mount, &lcdDisplay);
#endif

#if SUPPORT_GUIDED_STARTUP == 1
bool inStartup = true;        // Start with a guided startup
#else
bool inStartup = false;        // Start with a guided startup
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
  setDebugLevel(DEBUG_LEVEL);

  #if USE_GPS == 1
  GPS_SERIAL_PORT.begin(GPS_BAUD_RATE);
  #endif

  /////////////////////////////////
  //   Microstepping/driver pins
  /////////////////////////////////
  #if RA_STEPPER_TYPE == STEP_NEMA17  // RA driver startup (for A4988)
    #if RA_DRIVER_TYPE == GENERIC_DRIVER
      // include A4988 microstep pins
      //#error "Define Microstep pins and delete this error."
      digitalWrite(RA_EN_PIN, HIGH);
      digitalWrite(RA_MS0_PIN);  // MS0
      digitalWrite(RA_MS1_PIN);  // MS1
      digitalWrite(RA_MS2_PIN);  // MS2
      #endif
    #if RA_DRIVER_TYPE == TMC2209_STANDALONE
      // include TMC2209 Standalone pins
      pinMode(40, OUTPUT);
      digitalWrite(40, LOW);  // ENABLE, LOW to enable
      digitalWrite(41, HIGH);  // MS2
      digitalWrite(42, HIGH);  // MS1
      #endif
    #if RA_DRIVER_TYPE == TMC2209_UART
      // include TMC2209 UART pins  
      pinMode(RA_EN_PIN, OUTPUT);
      pinMode(RA_DIAG_PIN, INPUT);
      digitalWrite(RA_EN_PIN, LOW);  // Logic LOW to enable driver
      RA_SERIAL_PORT.begin(57600);  // Start HardwareSerial comms with driver
    #endif
  #endif
  #if DEC_STEPPER_TYPE == STEP_NEMA17  // DEC driver startup (for A4988)
    #if DEC_DRIVER_TYPE == GENERIC_DRIVER  // DEC driver startup (for A4988)
      digitalWrite(DEC_EN_PIN, HIGH);
      digitalWrite(DEC_MS0_PIN);  // MS1
      digitalWrite(DEC_MS1_PIN);  // MS2
      digitalWrite(DEC_MS2_PIN);  // MS3
    #endif
    #if DEC_DRIVER_TYPE == TMC2209_STANDALONE
      // include TMC2209 Standalone pins  TODO-----------------------
      //pinMode(40, OUTPUT);
      //digitalWrite(40, LOW);  // ENABLE, LOW to enable
      //digitalWrite(41, HIGH);  // MS2
      //digitalWrite(42, HIGH);  // MS1
    #endif
    #if DEC_DRIVER_TYPE == TMC2209_UART
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

  LOGV2(DEBUG_ANY, F("Hello, universe, this is OAT %s!"), Version);

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
  lcdDisplay.setCursor(0, 0);
  lcdDisplay.printLine(oatString(UI_OAT));
  lcdDisplay.setCursor(0, 1);
  lcdDisplay.printLine("     " + String(Version));

  if (LcdButtons::instance()->currentState() == btnDOWN){
    LOGV1(DEBUG_ANY, F("Erasing configuration in EEPROM!"));
    mount.clearConfiguration();
    while (LcdButtons::instance()->currentState() != btnNONE) {
      delay(10);
    }
  }

  #if HEADLESS_CLIENT == 0
    unsigned long now = millis();
  #endif
  // Create the command processor singleton
  LOGV1(DEBUG_ANY, F("Initialize LX200 handler..."));
  MeadeCommandProcessor::createProcessor(&mount, &lcdDisplay);

  #ifdef WIFI_ENABLED
    LOGV1(DEBUG_ANY, "Setup Wifi...");
    wifiControl.setup();
  #endif

  // Configure the mount
  LOGV1(DEBUG_ANY, F("Delay for a while to get UARTs booted..."));
  delay(1000);

  LOGV1(DEBUG_ANY, F("Configure RA stepper..."));
  // Set the stepper motor parameters
  #if RA_STEPPER_TYPE == STEP_28BYJ48 
    mount.configureRAStepper(FULLSTEP_MODE, RAmotorPin1, RAmotorPin2, RAmotorPin3, RAmotorPin4, RAspeed, RAacceleration);
  #elif RA_STEPPER_TYPE == STEP_NEMA17
    mount.configureRAStepper(DRIVER_MODE, RAmotorPin1, RAmotorPin2, RAspeed, RAacceleration);
  #else
    #error New stepper type? Configure it here.
  #endif

  LOGV1(DEBUG_ANY, F("Configure DEC stepper..."));
  #if DEC_STEPPER_TYPE == STEP_28BYJ48
    // LOGV1(DEBUG_ANY, F("Configure DEC stepper 28BYJ-48..."));
    mount.configureDECStepper(HALFSTEP_MODE, DECmotorPin1, DECmotorPin2, DECmotorPin3, DECmotorPin4, DECspeed, DECacceleration);
  #elif DEC_STEPPER_TYPE == STEP_NEMA17
    // LOGV1(DEBUG_ANY, F("Configure DEC stepper NEMA..."));
    mount.configureDECStepper(DRIVER_MODE, DECmotorPin1, DECmotorPin2, DECspeed, DECacceleration);
  #else
    #error New stepper type? Configure it here.
  #endif

  #if RA_DRIVER_TYPE == TMC2209_UART
    LOGV1(DEBUG_ANY, F("Configure RA driver..."));
    mount.configureRAdriver(&RA_SERIAL_PORT, R_SENSE, RA_DRIVER_ADDRESS, RA_RMSCURRENT, RA_STALL_VALUE);
  #endif
  #if DEC_DRIVER_TYPE == TMC2209_UART
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
  LOGV1(DEBUG_ANY, F("Read configuration..."));
  mount.readConfiguration();
  
  // Read other persisted values and set in mount
  DayTime haTime = DayTime(EPROMStore::read(1), EPROMStore::read(2), 0);

  mount.setHA(haTime);

  // For LCD screen, it's better to initialize the target to where we are (RA)
  mount.targetRA() = mount.currentRA();

  // Hook into the timers for periodic interrupts to run the steppers. 
  mount.startTimerInterrupts();

  // Start the tracker.
  LOGV1(DEBUG_ANY, F("Start Tracking..."));
  mount.startSlewing(TRACKING);

  #if HEADLESS_CLIENT == 0
    // LOGV1(DEBUG_ANY, "Setup menu system...");

    // // Create the LCD top-level menu items
    // lcdDisplay.addItem("RA", RA_Menu);
    // lcdDisplay.addItem("DEC", DEC_Menu);

    // #if SUPPORT_POINTS_OF_INTEREST == 1
    //   lcdDisplay.addItem("GO", POI_Menu);
    // #else
    //   lcdDisplay.addItem("GO", Home_Menu);
    // #endif

    // lcdDisplay.addItem("HA", HA_Menu);

    // #if SUPPORT_HEATING == 1
    //   lcdDisplay.addItem("HEA", Heat_Menu);
    // #endif

    // #if SUPPORT_MANUAL_CONTROL == 1
    //   lcdDisplay.addItem("CTRL", Control_Menu);
    // #endif

    // #if SUPPORT_CALIBRATION == 1
    //   lcdDisplay.addItem("CAL", Calibration_Menu);
    // #endif

    // #if SUPPORT_INFO_DISPLAY == 1
    //   lcdDisplay.addItem("INFO", Status_Menu);
    // #endif

    while (millis() - now < 750) {
      mount.loop();
    }

    // LOGV1(DEBUG_ANY, "Update display...");
    // lcdDisplay.updateDisplay();
  #else
    createMenuSystem(mainMenu);
  #endif // HEADLESS_CLIENT

  LOGV2(DEBUG_ANY, F("Size of Button is %d"), sizeof(Button));
  LOGV2(DEBUG_ANY, F("Size of NumberInput is %d"), sizeof(NumberInput));
  LOGV2(DEBUG_ANY, F("Size of Moubnt is %d"), sizeof(Mount));
  LOGV2(DEBUG_ANY, F("Size of MeadeCP is %d"), sizeof(MeadeCommandProcessor));
  LOGV1(DEBUG_ANY, F("Creating main menu..."));
  createMenuSystem(mainMenu);

  mount.bootComplete();
  LOGV1(DEBUG_ANY, F("Setup done!"));
}
