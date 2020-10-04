#pragma once

#if HEADLESS_CLIENT == 0
#if SUPPORT_MANUAL_CONTROL == 1
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
  
  return ret;
}

bool processControlKeys() {
  byte key;
  bool waitForRelease = false;

  // User must use SELECT to enter manual control.
  if (!inControlMode) {
    if (lcdButtons.keyChanged(&key)) {
      waitForRelease = true;
      if (key == btnSELECT) {
        inControlMode = true;
        mount.stopSlewing(ALL_DIRECTIONS);
      }
      else if (key == btnRIGHT) {
        lcdMenu.setNextActive();
      }
    }
    return waitForRelease;
  }

  if (confirmZeroPoint) {
    if (lcdButtons.keyChanged(&key)) {
      waitForRelease = true;
      if (key == btnSELECT) {
        if (setZeroPoint) {
          // Leaving Control Menu, so set stepper motor positions to zero.
          LOGV1(DEBUG_GENERAL, F("CTRL menu: Calling setHome(true)!"));
          mount.setHome(true);
          LOGV3(DEBUG_GENERAL, F("CTRL menu: setHome(true) returned: RA Current %s, Target: %f"), mount.RAString(CURRENT_STRING|COMPACT_STRING).c_str(), mount.RAString(TARGET_STRING | COMPACT_STRING).c_str());
          mount.startSlewing(TRACKING);
        }

        // Set flag to prevent resetting zero point when moving over the menu items
        inControlMode = false;

#if SUPPORT_GUIDED_STARTUP == 1
        if (startupState == StartupWaitForPoleCompletion) {
          startupState = StartupPoleConfirmed;
          inStartup = true;
          inControlMode = false;
        }
        else
#endif
        {
          lcdMenu.setNextActive();
        }

        confirmZeroPoint = false;
        setZeroPoint = true;
      }
      else if (key == btnLEFT) {
        setZeroPoint = !setZeroPoint;
      }
    }
    return waitForRelease;
  }

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
#if SUPPORT_GUIDED_STARTUP == 1
        if (startupState == StartupWaitForPoleCompletion) {
          startupState = StartupPoleConfirmed;
          inControlMode = false;
          confirmZeroPoint = false;
          waitForRelease = true;
          inStartup = true;
        }
        else
#endif
        {
          lcdMenu.setCursor(0, 0);
          lcdMenu.printMenu("Set home pos?");
          confirmZeroPoint = true;
          waitForRelease = true;
        }
      }
    }
    break;

    case btnNONE: {
      processKeyStateChanges(btnNONE, 0);
    }
    break;
  }

  return waitForRelease;
}


void printControlSubmenu() {
  if (!inControlMode) {
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
