void setup() {

  Serial.begin(57600); 
  
  lcd.begin(16, 2);
  lcd.createChar(0, DEG);
  lcd.createChar(1, min);
  lcd.createChar(2, sec);

  stepperRA.setMaxSpeed(RAspeed);
  stepperRA.setAcceleration(RAacceleration);
  stepperDEC.setMaxSpeed(DECspeed);
  stepperDEC.setAcceleration(600);
  stepperTRK.setMaxSpeed(10);
  stepperTRK.setAcceleration(250);
  stepperGUIDE.setMaxSpeed(50);
  stepperGUIDE.setAcceleration(100);

  inputcal = EEPROM.read(0);
  

}
