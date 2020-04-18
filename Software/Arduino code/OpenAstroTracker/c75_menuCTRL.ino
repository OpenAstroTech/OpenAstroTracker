#ifndef HEADLESS_CLIENT
#ifdef SUPPORT_MANUAL_CONTROL
bool confirmZeroPoint = false;
bool setZeroPoint = true;


#define LOOPS_TO_CONFIRM_KEY 10
byte loopsWithKeyPressed = 0;
byte keyPressed = btnNONE;


bool processKeyStateChanges(int key, int dir)
{
  bool ret = false;
  if (keyPressed != key) {
    loopsWithKeyPressed = 0;
    keyPressed = key;
  }
  else {
    if (loopsWithKeyPressed == LOOPS_TO_CONFIRM_KEY) {
      mount.stopSlewing(ALL_DIRECTIONS);
      mount.waitUntilStopped(ALL_DIRECTIONS);
      if (dir != 0) {
        mount.startSlewing(dir);
      }
      loopsWithKeyPressed++;      
      ret = true;
    }
    else if (loopsWithKeyPressed < LOOPS_TO_CONFIRM_KEY) {
      loopsWithKeyPressed++;
    }
  }
}

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
          mount.startSlewing(TRACKING);
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

  mount.loop();

  key = lcdButtons.currentState();
  switch (key) {
    case btnUP: {
        processKeyStateChanges(btnUP, NORTH);
      }
      break;

    case btnDOWN: {
        processKeyStateChanges(btnDOWN, SOUTH);
      }
      break;

    case btnLEFT: {
        processKeyStateChanges(btnLEFT, WEST);
      }
      break;

    case btnRIGHT: {
        processKeyStateChanges(btnRIGHT, EAST);
      }
      break;

    case btnSELECT: {
        if (processKeyStateChanges(btnSELECT, 0))
        {
          lcdMenu.setCursor(0, 0);
          lcdMenu.printMenu("Set home point?");
          confirmZeroPoint = true;
        }
      }
      break;

    case btnNONE: {
        processKeyStateChanges(btnNONE, 0);
      }
      break;
  }

  mount.loop();

  return false;
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
#endif
#endif
