
void displayStepperPosition() {
  lcdMenu.setCursor(0, 1);
  String disp ;
  if (totalDECMove > 0)              {
    float decDist = 100.0 - 100.0 * abs(stepperDEC.distanceToGo()) / totalDECMove;
    sprintf(scratchBuffer, "DEC: %d%%", (int)decDist);
    //disp = "DEC:" + String((int)floor(decDist)) + "% ";
    disp = String(scratchBuffer);
  }
  else {
    disp = "D:" + String(stepperDEC.currentPosition());
  }
  if (totalRAMove > 0) {
    float raDist = 100.0 - 100.0 * abs(stepperRA.distanceToGo()) / totalRAMove;
    sprintf(scratchBuffer, "RA: %d%%", (int)raDist);
    disp = disp + String(scratchBuffer);
  }
  else {
    disp = disp + " R:" + String(stepperRA.currentPosition());
  }

  lcdMenu.printMenu(disp);
}

void displayStepperPositionThrottled() {
  long elapsed = millis() - lastDisplayUpdate;
  if (elapsed > DISPLAY_UPDATE_TIME) {
    displayStepperPosition();
    lastDisplayUpdate = millis();
  }
}

// Stop the steppers, allowing their decelerations to process (Blocking)
void stopSteppers() {
  // Is it running?
  stepperRA.stop();
  stepperDEC.stop();
  while (stepperRA.isRunning() || stepperDEC.isRunning()) {
    stepperRA.run();
    stepperDEC.run();
    runTracker();
    displayStepperPositionThrottled();
  }
}

// Stop one of the steppers, allowing its decelerations to process and update controlState (Blocking)
// This function is meant for use by the CTRL menu
bool stopStepper(byte mask, bool useRA) {
  // Is it running?
  if (controlState & mask)
  {
    // Yes, so ask for a stop
    if (useRA) {
      stepperRA.stop();
    }
    else {
      stepperDEC.stop();
    }

    // Allow it to run until deceleration is complete
    if (useRA) {
      while (stepperRA.isRunning()) {
        stepperRA.run();
        stepperDEC.run();
        runTracker();
        displayStepperPositionThrottled();
      }
    }
    else {
      while (stepperDEC.isRunning()) {
        stepperRA.run();
        stepperDEC.run();
        runTracker();
        displayStepperPositionThrottled();
      }
    }

    controlState = controlState & ~mask;
    return true;
  }

  return false;
}

// Move stepper motors to target. Called from RA or DEC menu after pressing SELECT (Blocking)
// Displays a percentage progress
void moveSteppersToTarget() {

  float decTotal = 1.0f * abs(stepperDEC.distanceToGo());
  float raTotal = 1.0f * abs(stepperRA.distanceToGo());
  while (stepperDEC.distanceToGo() != 0 || stepperRA.distanceToGo() != 0) {
    stepperRA.run();
    stepperDEC.run();

    // Run the TRK stepper (same one at slow speed)
    runTracker();

    displayStepperPositionThrottled();
  }

  // Update resting position
  displayStepperPosition();
}

void startMoveSteppersToTargetAsync() {
  totalDECMove = 1.0f * abs(stepperDEC.distanceToGo());
  totalRAMove = 1.0f * abs(stepperRA.distanceToGo());
}

// Move stepper motors to target (non-blocking)
bool moveSteppersToTargetAsync() {
  bool stillRunning = false;
  if (stepperDEC.isRunning() || stepperRA.isRunning()) {
    stillRunning = true;
    stepperRA.run();
    stepperDEC.run();

    // Run the TRK stepper (same one at slow speed)
    runTracker();
    displayStepperPositionThrottled();
  }
  else {
    // Make sure we do one last update when the steppers have stopped.
    displayStepperPosition();
  }

  return stillRunning;
}

void ShowStatusMessage(String message) {
  lcdMenu.setCursor(0, 1);
  lcdMenu.printMenu(message);
  long now = millis();
  while (millis() - now < 750) {
    runTracker();
  }
}
