void setup() {

  Serial.begin(9600); 
  //Serial.begin(9600);
  //BT.begin(9600);

  
  lcd.begin(16, 2);
  lcd.createChar(0, DEG);
  lcd.createChar(1, min);
  lcd.createChar(2, sec);

  stepperRA.setMaxSpeed(RAspeed);
  stepperRA.setAcceleration(RAacceleration);
  stepperDEC.setMaxSpeed(DECspeed);
  stepperDEC.setAcceleration(DECacceleration);
  stepperTRK.setMaxSpeed(10);
  stepperTRK.setAcceleration(2500);
  stepperGUIDE.setMaxSpeed(50);
  stepperGUIDE.setAcceleration(100);

  inputcal = EEPROM.read(0);
  hourHA = EEPROM.read(1);
  minHA = EEPROM.read(2);
  

}
