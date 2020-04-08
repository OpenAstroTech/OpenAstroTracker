bool confirmZeroPoint = false;
bool setZeroPoint = true;

bool processControlKeys() {
  byte key;

  // User must use SELECT to enter manual control.
  if (!inControlMode ) {
    if (lcdButtons.keyChanged(key)) {
      if (key == btnSELECT) {
        inControlMode = true;
        mount.stopSlewing(ALL_DIRECTIONS);
      } else if (key == btnRIGHT) {
        lcdMenu.setNextActive();
      }
    }
    return true;
  }

  if (confirmZeroPoint) {
    if (lcdButtons.keyChanged(key)) {
      if (key == btnSELECT) {
        if (setZeroPoint) {
          // Leaving Control Menu, so set stepper motor positions to zero.
          mount.setHome();
        }

        // Set flag to prevent resetting zero point when moving over the menu items
        inControlMode = false;

#ifdef SUPPORT_GUIDED_STARTUP
        if (startupState == StartupWaitForPoleCompletion) {
          startupState = StartupPoleConfirmed;
          inStartup = true;
        }
        else 
#endif        
        {
          lcdMenu.setNextActive();
        }

        confirmZeroPoint = false;
        setZeroPoint = true;
      } else if (key == btnLEFT) {
        setZeroPoint = !setZeroPoint;
      }
    }
    return true;
  }

  if (lcdButtons.keyChanged(key)) {
    switch (key) {
      case btnUP: {
          if (!mount.isSlewingDEC()) {
            mount.startSlewing(NORTH);
          } else {
            mount.stopSlewing(NORTH | SOUTH);
          }
        }
        break;

      case btnDOWN: {
          if (!mount.isSlewingDEC()) {
            mount.startSlewing(SOUTH);
          } else {
            mount.stopSlewing(NORTH | SOUTH);
          }
        }
        break;

      case btnLEFT: {
          if (!mount.isSlewingRA()) {
            mount.startSlewing(WEST);
          } else {
            mount.stopSlewing(EAST | WEST);
          }
        }
        break;

      case btnRIGHT: {
          if (!mount.isSlewingRA()) {
            mount.startSlewing(EAST);
          } else {
            mount.stopSlewing(EAST | WEST);
          }
        }
        break;

      case btnSELECT: {
          mount.stopSlewing(ALL_DIRECTIONS);
          mount.waitUntilStopped(ALL_DIRECTIONS);
          lcdMenu.setCursor(0, 0);
          lcdMenu.printMenu("Set home point?");
          confirmZeroPoint = true;
        }
        break;
    }
  }

  return true;
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
