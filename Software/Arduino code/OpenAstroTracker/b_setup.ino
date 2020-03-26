
// Create the menu variable
LcdMenu lcdMenu(&lcd, 16);

// Create the variables to track RA time, RA display time and HA time
DayTime RATime;
DayTime RADisplayTime;
DayTime HATime;

void setup() {

  //Serial.begin(38400);
  Serial.begin(9600);
  //BT.begin(9600);

#ifdef DEBUG_MODE
  log("Hello World!");
#endif

  // Initialize the LCD (16x2 characters)
  lcd.begin(16, 2);

  // Create special characters for degrees, minutes, seconds
  lcd.createChar(0, DegreesBitmap);
  lcd.createChar(1, MinutesBitmap);
  lcd.createChar(2, SecondsBitmap);

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
  stepperGUIDE.setMaxSpeed(50);
  stepperGUIDE.setAcceleration(100);

  // Read persisted values
  inputcal = EEPROM.read(0);
  speedcalibration = speed + inputcal / 10000;
  HATime = DayTime(EEPROM.read(1), EEPROM.read(2), 0);

#ifdef DEBUG_MODE
  logv("HATime = %s", HATime.ToString().c_str());
#endif

  // Create the menu items
  lcdMenu.addItem("RAs", RA_Menu);
  lcdMenu.addItem("DEC", DEC_Menu);
  lcdMenu.addItem("HOME", Home_Menu);
  lcdMenu.addItem("HA", HA_Menu);
  if (north) {
    lcdMenu.addItem("POL", Polaris_Menu);
  }
#ifdef SUPPORT_HEATING  
  lcdMenu.addItem("HEAT", Heat_Menu);
#endif
  lcdMenu.addItem("CTRL", Control_Menu);
  lcdMenu.addItem("CAL", Calibration_Menu);

  // Show a splash screen
  lcd.setCursor(0, 0);
  lcd.print("OpenAstroTracker");
  lcd.setCursor(0, 1);
  lcd.print("     " + version);
  delay(1750);

  
}
