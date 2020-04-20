#ifndef HEADLESS_CLIENT

// HIGHLIGHT states allow you to pick one of the three sub functions.
#define HIGHLIGHT_POLAR 1
#define HIGHLIGHT_SPEED 2
#define HIGHLIGHT_DRIFT 3

// Polar calibration goes through these three states:
//  4 - moving to RA of Polaris and then waiting on confirmation that Polaris is centered
//  5 - moving to DEC beyond Polaris and waiting on confirmation that Polaris is centered
//  6 - moving back to home position
#define POLAR_CALIBRATION_WAIT 4
#define POLAR_CALIBRATION_GO  5
#define POLAR_CALIBRATION_WAIT_HOME  6

// Speed calibration only has one state, allowing you to adjust the speed with UP and DOWN
#define SPEED_CALIBRATION 7

// Drift calibration goes through 4 states
// 8 - Display four durations and wait for the user to select one
// 9 - Start the calibration run after user presses SELECT. This state waits 1.5s, takes duration time
//     to slew east in half the time selected, then waits 1.5s and slews west in the same duration, and waits 1.5s.
#define DRIFT_CALIBRATION_GET_DURATION 8
#define DRIFT_CALIBRATION_RUNNING 9

// Start off with Polar Alignment higlighted.
byte calState = HIGHLIGHT_POLAR;

// The index of durations that the user has selected.
byte driftSubIndex = 1;

// The requested total duration of the drift alignment run.
byte driftDuration = 0;

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
  } else if (calState == DRIFT_CALIBRATION_RUNNING) {
    lcdMenu.setCursor(0, 1);
    lcdMenu.printMenu("Pause 1.5s ...");
    mount.stopSlewing(TRACKING);
    mount.delay(1500);

    lcdMenu.setCursor(0, 1);
    lcdMenu.printMenu("Eastward pass...");
    mount.runDriftAlignmentPhase(EAST, driftDuration);

    lcdMenu.setCursor(0, 1);
    lcdMenu.printMenu("Pause 1.5s ...");
    mount.delay(1500);

    lcdMenu.setCursor(0, 1);
    lcdMenu.printMenu("Westward pass...");
    mount.runDriftAlignmentPhase(WEST, driftDuration);

    lcdMenu.setCursor(0, 1);
    lcdMenu.printMenu("Done. Pause 1.5s");
    mount.delay(1500);
    mount.runDriftAlignmentPhase(0, 0);

    mount.startSlewing(TRACKING);
    calState = HIGHLIGHT_DRIFT;
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
        else if (key == btnUP) calState = HIGHLIGHT_DRIFT;
        else if (key == btnSELECT) {
          calState = POLAR_CALIBRATION_WAIT;

          // Move the RA to that of Polaris. Moving to this RA aligns the DEC axis such that
          // it swings along the line between Polaris and the Celestial Pole.
          mount.targetRA() = DayTime(PolarisRAHour, PolarisRAMinute, PolarisRASecond);
          // Account for the current settings.
          mount.targetRA().addTime(mount.getHACorrection());
          mount.targetRA().subtractTime(mount.HA());

          // Now set DEC to move to Home position
          mount.targetDEC() = DegreeTime(90 - (NORTHERN_HEMISPHERE ? 90 : -90), 0, 0);
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
          mount.targetDEC() = DegreeTime(89 - (NORTHERN_HEMISPHERE ? 90 : -90), 21, 3);
          mount.startSlewingToTarget();
        }
        else if (key == btnRIGHT) {
          lcdMenu.setNextActive();
          calState = HIGHLIGHT_POLAR;
        }
        break;

      case HIGHLIGHT_SPEED:
        if (key == btnDOWN) calState = HIGHLIGHT_DRIFT;
        if (key == btnUP) calState = HIGHLIGHT_POLAR;
        else if (key == btnSELECT) calState = SPEED_CALIBRATION;
        else if (key == btnRIGHT) {
          lcdMenu.setNextActive();
          calState = HIGHLIGHT_POLAR;
        }
        break;
        
      case HIGHLIGHT_DRIFT:
        if (key == btnDOWN) calState = HIGHLIGHT_POLAR;
        if (key == btnUP) calState = HIGHLIGHT_SPEED;
        else if (key == btnSELECT) calState = DRIFT_CALIBRATION_GET_DURATION;
        else if (key == btnRIGHT) {
          lcdMenu.setNextActive();
          calState = HIGHLIGHT_POLAR;
        }
        break;
        
      case DRIFT_CALIBRATION_GET_DURATION :
        if (key == btnDOWN || key == btnLEFT) {
          driftSubIndex = adjustWrap(driftSubIndex, 1, 0, 3);
        }
        if (key == btnUP) {
          driftSubIndex = adjustWrap(driftSubIndex, -1, 0, 3);
        }
        if (key == btnSELECT) {
          // Take off 6s padding time. 1.5s start pause, 1.5s pause in the middle and 1.5s end pause and general time slop.
          // These are the times for one way. So total time is 2 x duration + 4.5s
          int duration[] = { 27, 57, 87, 147};
          driftDuration = duration[driftSubIndex];
          calState = DRIFT_CALIBRATION_RUNNING;
        }
        else if (key == btnRIGHT) {
          // RIGHT cancels duration selection and returns to menu
          calState = HIGHLIGHT_DRIFT;
          driftSubIndex = 1;
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
    case HIGHLIGHT_DRIFT:
      lcdMenu.printMenu(">Drift alignment");
    case DRIFT_CALIBRATION_GET_DURATION:
      sprintf(scratchBuffer, " 1m  2m  3m  5m");
      scratchBuffer[driftSubIndex * 4] = '>';
      lcdMenu.printMenu(scratchBuffer);
      break;
  }
}
#endif
