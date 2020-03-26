
bool confirmZeroPoint = false;
bool setZeroPoint = true;

void processControlKeys(int key) {

  // User must use SELECT to enter manual control.
  if (!inControlMode ) {
    if (key == btnSELECT) {
      inControlMode = true;
      Serial.println("CTRL: Reset totals!");
      totalDECMove = 0;
      totalRAMove = 0;
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
        stepperRA.setCurrentPosition(0);
        stepperDEC.setCurrentPosition(0);
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
        if (lastKey != btnUP)
        {
          if (!stopStepper(StateMaskDEC, false))
          {
            stepperDEC.moveTo(-22000);
            controlState = controlState | StateMaskUp;
          }
        }
      }
      break;

    case btnDOWN: {
        if (lastKey != btnDOWN)
        {
          if (!stopStepper(StateMaskDEC, false))
          {
            // No, so start it.
            stepperDEC.moveTo(22000);
            controlState = controlState | StateMaskDown;
          }
        }
      }
      break;

    case btnLEFT: {
        if (lastKey != btnLEFT)
        {
          if (!stopStepper(StateMaskRA, true))
          {
            stepperRA.moveTo(15000);
            controlState = controlState | StateMaskLeft;
          }
        }
      }
      break;

    case btnRIGHT: {
        if (lastKey != btnRIGHT)
        {
          if (!stopStepper(StateMaskRA, true))
          {
            stepperRA.moveTo(-15000);
            controlState = controlState | StateMaskRight;
          }
        }
      }
      break;

    case btnSELECT: {
        stopStepper(StateMaskDEC, &stepperDEC);
        stopStepper(StateMaskRA, &stepperRA);

        lcdMenu.setCursor(0, 0);
        lcdMenu.printMenu("Set home point?");
        confirmZeroPoint = true;
        waitForButtonRelease = true;
      }
      break;
  }

  lastKey = key;

  // If we've set a target keep moving it there (bit by bit, not waiting).
  moveSteppersToTargetAsync();
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
  } else {
    if (!stepperDEC.isRunning() && !stepperRA.isRunning()) {
      lcdMenu.printMenu("D:" + String(stepperDEC.currentPosition()) + " R:" + stepperRA.currentPosition());
    }
  }
}
