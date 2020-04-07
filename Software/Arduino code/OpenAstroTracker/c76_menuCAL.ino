#define HIGHLIGHT_POLAR 1
#define HIGHLIGHT_SPEED 2
#define POLAR_CALIBRATION 3
#define SPEED_CALIBRATION 4

byte calState = HIGHLIGHT_POLAR;

bool processCalibrationKeys() {
  byte key;
  bool waitForRelease = false;
  bool checkForKeyChange = true;

  if (calState == SPEED_CALIBRATION)
  {
    if (lcdButtons.currentState() == btnUP)  {
      inputcal += 1;  //0.0001;
      mount.setSpeedCalibration(speed + inputcal / 10000);
      mount.delay(calDelay);
      calDelay = max(5, 0.96 * calDelay);
      checkForKeyChange = false;
    }
    else if (lcdButtons.currentState() == btnDOWN)  {
      inputcal -= 1 ; //0.0001;
      mount.setSpeedCalibration(speed + inputcal / 10000);
      mount.delay(calDelay);
      calDelay = max(5, 0.96 * calDelay);
      checkForKeyChange = false;
    }
    else {
      calDelay = 150;
    }
  }

  if (checkForKeyChange && lcdButtons.keyChanged(key)) {
    waitForRelease = true;

    switch (calState) {
      case POLAR_CALIBRATION: {
          if (key == btnSELECT) {
            mount.setTargetToHome();
            mount.startSlewingToTarget();
            lcdMenu.setNextActive();
            calState = HIGHLIGHT_POLAR;
          }
          if (key == btnRIGHT) {
            lcdMenu.setNextActive();
            calState = HIGHLIGHT_POLAR;
          }
        }
        break;
      case SPEED_CALIBRATION: {
          // UP and DOWN are handled above
          if (key == btnSELECT) {
            EEPROM.update(0, inputcal);
            mount.setSpeedCalibration(speed + inputcal / 10000);
            lcdMenu.printMenu("Stored.");
            mount.delay(500);
            calState = HIGHLIGHT_SPEED;
          }
          else if (key == btnRIGHT) {
            lcdMenu.setNextActive();
            calState = HIGHLIGHT_POLAR;
          }
        }
        break;

      case HIGHLIGHT_POLAR:
        if (key == btnDOWN) calState = HIGHLIGHT_SPEED;
        else if (key == btnSELECT) {
          calState = POLAR_CALIBRATION;
          
          // Move the RA to that of Polaris. Moving to this RA aligns the DEC axis such that
          // it swings along the line between Polaris and the Celestial Pole.
          mount.targetRA() = DayTime(2, 57, 56); // This is Polaris RA. 
          // Account for the current settings.
          mount.targetRA().addTime(mount.getHACorrection());
          mount.targetRA().subtractTime(mount.HA());
          
          // Now set DEC to move the same distance past Polaris as it is from the Celestial Pole. That equates to 88deg 42' 6".
          mount.targetDEC() = DegreeTime(88 - (NORTHERN_HEMISPHERE ? 90 : -90), 42, 6);
          mount.startSlewingToTarget();
        }
        else if (key == btnRIGHT) {
          lcdMenu.setNextActive();
          calState = HIGHLIGHT_POLAR;
        }
        break;

      case HIGHLIGHT_SPEED:
        if (key == btnDOWN) calState = HIGHLIGHT_POLAR;
        else if (key == btnSELECT) calState = SPEED_CALIBRATION;
        else if (key == btnRIGHT) {
          lcdMenu.setNextActive();
          calState = HIGHLIGHT_POLAR;
        }
        break;
    }
  }

  return waitForRelease;
}

void printCalibrationSubmenu() {
  char scratchBuffer[20];
  switch (calState)  {
    case HIGHLIGHT_POLAR :
      lcdMenu.printMenu(">Polar alignment");
      break;
    case HIGHLIGHT_SPEED:
      lcdMenu.printMenu(">Speed calibratn");
      break;
    case POLAR_CALIBRATION :
      if (!mount.isSlewingRAorDEC()) {
        lcdMenu.printMenu("Centr on Polaris");
      }
      break;
    case SPEED_CALIBRATION :
      sprintf(scratchBuffer, "SpdFctr: ");
      dtostrf(mount.getSpeedCalibration(), 6, 4, &scratchBuffer[9]);
      lcdMenu.printMenu(scratchBuffer);
      break;
  }
}
