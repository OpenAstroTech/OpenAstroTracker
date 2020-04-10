#define HIGHLIGHT_POLAR 1
#define HIGHLIGHT_SPEED 2
#define POLAR_CALIBRATION_WAIT 3
#define POLAR_CALIBRATION_GO  4
#define POLAR_CALIBRATION_WAIT_HOME  5
#define SPEED_CALIBRATION 6

byte calState = HIGHLIGHT_POLAR;

bool processCalibrationKeys() {
  byte key;
  bool waitForRelease = false;
  bool checkForKeyChange = true;

  if (calState == SPEED_CALIBRATION)
  {
    if (lcdButtons.currentState() == btnUP)  {
      if (inputcal < 32760) { // Don't overflow 16 bit signed
        inputcal += 1;  //0.0001;
        mount.setSpeedCalibration(speed + inputcal / 10000);
      }
      mount.delay(calDelay);
      calDelay = max(5, 0.96 * calDelay);
      checkForKeyChange = false;
    }
    else if (lcdButtons.currentState() == btnDOWN)  {
      if (inputcal > -32760) { // Don't overflow 16 bit signed
        inputcal -= 1 ; //0.0001;
        mount.setSpeedCalibration(speed + inputcal / 10000);
      }
      mount.delay(calDelay);
      calDelay = max(5, 0.96 * calDelay);
      checkForKeyChange = false;
    }
    else {
      calDelay = 150;
    }
  }
  else if (calState == POLAR_CALIBRATION_WAIT_HOME) {
    if (!mount.isSlewingRAorDEC()) {
      lcdMenu.updateDisplay();
      calState = HIGHLIGHT_POLAR;
    }
  }



  if (checkForKeyChange && lcdButtons.keyChanged(key)) {
    waitForRelease = true;

    switch (calState) {

      case POLAR_CALIBRATION_GO: {
          if (key == btnSELECT) {
            lcdMenu.printMenu("Aligned, homing");
            mount.delay(600);
            mount.setTargetToHome();
            mount.startSlewingToTarget();
            calState = POLAR_CALIBRATION_WAIT_HOME;
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
            int cal = floor(inputcal);
            EEPROM.update(0, cal & 0x00FF);
            EEPROM.update(3, (cal & 0xFF00) >> 8);
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
        else if (key == btnUP) calState = HIGHLIGHT_SPEED;
        else if (key == btnSELECT) {
          calState = POLAR_CALIBRATION_WAIT;

          // Move the RA to that of Polaris. Moving to this RA aligns the DEC axis such that
          // it swings along the line between Polaris and the Celestial Pole.
          mount.targetRA() = DayTime(2, 57, 56); // This is Polaris RA.
          // Account for the current settings.
          mount.targetRA().addTime(mount.getHACorrection());
          mount.targetRA().subtractTime(mount.HA());

          // Now set DEC to move to Polaris
          mount.targetDEC() = DegreeTime(89 - (NORTHERN_HEMISPHERE ? 90 : -90), 21, 3);
          mount.startSlewingToTarget();
        } else if (key == btnRIGHT) {
          lcdMenu.setNextActive();
        }
        break;

      case POLAR_CALIBRATION_WAIT:
        if (key == btnSELECT) {
          calState = POLAR_CALIBRATION_GO;

          // RA is already set. Now set DEC to move the same distance past Polaris as
          // it is from the Celestial Pole. That equates to 88deg 42' 6".
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
        if (key == btnUP) calState = HIGHLIGHT_POLAR;
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
    case POLAR_CALIBRATION_WAIT_HOME:
    case POLAR_CALIBRATION_WAIT:
    case POLAR_CALIBRATION_GO :
      if (!mount.isSlewingRAorDEC()) {
        lcdMenu.setCursor(0, 0);
        lcdMenu.printMenu("Centr on Polaris");
        lcdMenu.setCursor(0, 1);
        lcdMenu.printMenu(">Centered");
      }
      break;

    case HIGHLIGHT_SPEED:
      lcdMenu.printMenu(">Speed calibratn");
      break;
    case SPEED_CALIBRATION :
      sprintf(scratchBuffer, "SpdFctr: ");
      dtostrf(mount.getSpeedCalibration(), 6, 4, &scratchBuffer[9]);
      lcdMenu.printMenu(scratchBuffer);
      break;
  }
}
