
void processControlKeys(int key) {

  // User must use SELECT to enter manual control.
  if (!inControlMode ) {
    if (key == btnSELECT) {
      inControlMode = true;
      totalDECMove = 0;
      totalRAMove = 0;
    } else if (key == btnRIGHT) {
      lcdMenu.setNextActive();
    }
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
            stepperDEC.moveTo(11000);
            controlState = controlState | StateMaskDown;
          }
        }
        waitForButtonRelease = false;
      }
      break;

    case btnSELECT: {
        if (lastKey != btnSELECT)
        {
          if (!stopStepper(StateMaskRA, true))
          {
            stepperRA.moveTo(15000);
            controlState = controlState | StateMaskLeft;
          }
        }
        waitForButtonRelease = false;
      }
      break;

    case btnLEFT: {
        if (lastKey != btnLEFT)
        {
          if (!stopStepper(StateMaskRA, true))
          {
            stepperRA.moveTo(-15000);
            controlState = controlState | StateMaskRight;
          }
        }
        waitForButtonRelease = false;
      }
      break;

    case btnRIGHT: {
        stopStepper(StateMaskDEC, &stepperDEC);
        stopStepper(StateMaskRA, &stepperRA);

        // Leaving Control Menu, so set stepper motor positions to zero.
        stepperRA.setCurrentPosition(0);
        stepperDEC.setCurrentPosition(0);

        // Set flag to prevent menu selection from resetting control mode
        inControlMode = false;

        lcdMenu.setNextActive();
      }
      break;

    case btnNONE: {
      }
  }

  lastKey = key;

  // If we've set a target keep moving it there (bit by bit, not waiting).
  moveSteppersToTargetAsync();
}

void printControlSubmenu() {
  if (!inControlMode ) {
    lcdMenu.printMenu(">Manual control");
  }
  else {
    if (!stepperDEC.isRunning() && !stepperRA.isRunning()) {
      lcdMenu.printMenu(format("D:%l R:%l", stepperDEC.currentPosition(), stepperRA.currentPosition()));
    }
  }
}
