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
    }
    return true;
  }

  if (lcdButtons.keyChanged(key)) {
    //Serial.print("KEY CHANGE!");
    switch (key) {
      case btnUP: {
          //Serial.print("KEY UP:");
          if (!mount.isSlewingDEC()) {
            //Serial.println(" Go N");
            mount.startSlewing(NORTH);
          } else {
            //Serial.println(" Stop NS");
            mount.stopSlewing(NORTH | SOUTH);
          }
        }
        break;

      case btnDOWN: {
          //Serial.print("KEY DN:");
          if (!mount.isSlewingDEC()) {
            //Serial.println(" Go S");
            mount.startSlewing(SOUTH);
          } else {
            //Serial.println(" Stop NS");
            mount.stopSlewing(NORTH | SOUTH);
          }
        }
        break;

      case btnLEFT: {
          //Serial.print("KEY LF:");
          if (!mount.isSlewingRA()) {
            //Serial.println(" Go W");
            mount.startSlewing(WEST);
          } else {
            //Serial.println(" Stop EW");
            mount.stopSlewing(EAST | WEST);
          }
        }
        break;

      case btnRIGHT: {
          //Serial.print("KEY RT:");
          if (!mount.isSlewingRA()) {
            //Serial.println(" Go E");
            mount.startSlewing(EAST);
          } else {
            //Serial.println(" Stop EW");
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
