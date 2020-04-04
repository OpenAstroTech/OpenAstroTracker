
bool confirmZeroPoint = false;
bool setZeroPoint = true;

void processControlKeys(int key) {

  // User must use SELECT to enter manual control.
  if (!inControlMode ) {
    if (key == btnSELECT) {
      inControlMode = true;
      mount.stopSlewing(ALL_DIRECTIONS);
    } else if (key == btnRIGHT) {
      lcdMenu.setNextActive();
    }
    return;
  }

  if (confirmZeroPoint) {
    if (key == btnSELECT)
    {
      if (setZeroPoint) {
        // Leaving Control Menu, so set stepper motor positions to zero.
        mount.setHome();
      }

      // Set flag to prevent resetting zero point when moving over the menu items
      inControlMode = false;

      if (startupState == StartupWaitForPoleCompletion) {
        startupState = StartupPoleConfirmed;
        inStartup = true;
      }
      else {
        lcdMenu.setNextActive();
      }

      confirmZeroPoint = false;
      setZeroPoint = true;
    } else if (key == btnLEFT) {
      setZeroPoint = !setZeroPoint;
    }

    waitForButtonRelease = true;
    return;
  }

  waitForButtonRelease = false;
  switch (key) {
    case btnUP: {
        if (lastKey != btnUP) {
          if (!mount.isSlewingDEC()) {
            mount.startSlewing(NORTH);
          } else {
            mount.stopSlewing(NORTH | SOUTH);
          }
        }
      }
      break;

    case btnDOWN: {
        if (lastKey != btnDOWN) {
          if (!mount.isSlewingDEC()) {
            mount.startSlewing(SOUTH);
          } else {
            mount.stopSlewing(NORTH | SOUTH);
          }
        }
      }
      break;

    case btnLEFT: {
        if (lastKey != btnLEFT) {
          if (!mount.isSlewingRA()) {
            mount.startSlewing(WEST);
          } else {
            mount.stopSlewing(EAST | WEST);
          }
        }
      }
      break;

    case btnRIGHT: {
        if (lastKey != btnRIGHT) {
          if (!mount.isSlewingRA()) {
            mount.startSlewing(EAST);
          } else {
            mount.stopSlewing(EAST | WEST);
          }
        }
      }
      break;

    case btnSELECT: {
        mount.stopSlewing(ALL_DIRECTIONS);
        mount.waitUntilStopped(ALL_DIRECTIONS);
        lcdMenu.setCursor(0, 0);
        lcdMenu.printMenu("Set home point?");
        confirmZeroPoint = true;
        waitForButtonRelease = true;
      }
      break;
  }

  lastKey = key;
}

void printControlSubmenu() {
  if (!inControlMode ) {
    lcdMenu.printMenu(">Manual control");
  }
  else if (confirmZeroPoint) {
    String disp = " Yes  No  ";
    disp.setCharAt(setZeroPoint ? 0 : 5, '>');
    disp.setCharAt(setZeroPoint ? 4 : 8, '<');
    lcdMenu.printMenu(disp);
  }
  else {
    mount.displayStepperPositionThrottled();
  }
}
