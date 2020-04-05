bool processCalibrationKeys() {
  byte key;
  bool waitForRelease = false;

  if (lcdButtons.currentState() == btnUP)  {
    inputcal += 1;  //0.0001;
    mount.setSpeedCalibration(speed + inputcal / 10000);
    mount.delay(calDelay);
    calDelay = max(5, 0.96 * calDelay);
  }
  else if (lcdButtons.currentState() == btnDOWN)  {
    inputcal -= 1 ; //0.0001;
    mount.setSpeedCalibration(speed + inputcal / 10000);
    mount.delay(calDelay);
    calDelay = max(5, 0.96 * calDelay);
  }
  else {
    calDelay = 150;
    if (lcdButtons.keyChanged(key)) {
      waitForRelease = true;
      switch (key) {
        case btnSELECT: {
            EEPROM.update(0, inputcal);
            mount.setSpeedCalibration(speed + inputcal / 10000);
          }
          break;

        case btnRIGHT: {
            lcdMenu.setNextActive();
          }
          break;

        case btnNONE: {
          }
          break;
      }
    }
  }
  
  return waitForRelease;
}

void printCalibrationSubmenu() {
  char scratchBuffer[20];
  sprintf(scratchBuffer, "SpdFctr: ");
  dtostrf(mount.getSpeedCalibration(), 6, 4, &scratchBuffer[9]);
  lcdMenu.printMenu(scratchBuffer);
}
