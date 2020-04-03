// Create the LCD menu variable and initialize the LCD (16x2 characters)
LcdMenu lcdMenu(16, 2);

// Create the variables to track RA time, RA display time and HA time
DayTime RATime;
DayTime RADisplayTime;
DayTime HATime;
DayTime HACorrection;

void setup() {

  //Serial.begin(38400);
  Serial.begin(57600);
  //BT.begin(9600);

#ifdef DEBUG_MODE
  log("Hello World!");
#endif

  // Not sure if this is neeeded
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);

  // Configure stepper limits
  stepperRA.setMaxSpeed(RAspeed);
  stepperRA.setAcceleration(RAacceleration);
  stepperDEC.setMaxSpeed(DECspeed);
  stepperDEC.setAcceleration(DECacceleration);
  stepperTRK.setMaxSpeed(10);
  stepperTRK.setAcceleration(2500);

  // Read persisted values
  inputcal = EEPROM.read(0);
  speedCalibration = speed + inputcal / 10000;
  HATime = DayTime(EEPROM.read(1), EEPROM.read(2), 0);
  HACorrection.set(HATime);
  HACorrection.addTime(-h, -m, -s);
  lastHAset = millis();

#ifdef DEBUG_MODE
  logv("HATime = %s", HATime.ToString().c_str());
#endif

  // Create the menu items
  lcdMenu.addItem("RA", RA_Menu);
  lcdMenu.addItem("DEC", DEC_Menu);
  lcdMenu.addItem("POI", POI_Menu);
  lcdMenu.addItem("HOME", Home_Menu);
  lcdMenu.addItem("HA", HA_Menu);
#ifdef SUPPORT_HEATING
  lcdMenu.addItem("HEA", Heat_Menu);
#endif
  lcdMenu.addItem("CTRL", Control_Menu);
  lcdMenu.addItem("CAL", Calibration_Menu);
  lcdMenu.addItem("INFO", Status_Menu);

  // Show a splash screen
  lcdMenu.setCursor(0, 0);
  lcdMenu.printMenu("OpenAstroTracker");
  lcdMenu.setCursor(0, 1);
  lcdMenu.printMenu("     " + version);
  delay(1750);

  doCalculations();
  stepperTRK.setSpeed(trackingSpeed);
}
