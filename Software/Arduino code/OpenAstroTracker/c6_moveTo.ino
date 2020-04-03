
float currentRA() {
  float targetRA = RATime.getTotalHours();
  // Serial.print("TargetRA: " + String(targetRA, 4));
  if (!stepperRA.isRunning()) {
    return targetRA;
  }
  float degreesToGo = 1.0 * stepperRA.distanceToGo() / RAStepsPerDegree;
  // Serial.print("  Deg2Go: " + String(degreesToGo , 4));
  float hoursToGo = degreesToGo / 15.0;
  // Serial.print("  Hrs2Go: " + String(hoursToGo , 4));
  float currentHour = targetRA - hoursToGo;
  // Serial.println("  CurHrs: " + String(currentHour, 4));
  return currentHour;
}

float currentDEC() {
  float targetDEC = 1.0 * degreeDEC + minDEC / 60.0 + secDEC / 3600.0;
  // Serial.print("TargetDEC: " + String(targetDEC, 4));
  if (!stepperDEC.isRunning()) {
    return targetDEC;
  }
  float degreesToGo = 1.0 * stepperDEC.distanceToGo() / DECStepsPerDegree;
  // Serial.print("  Deg2Go: " + String(degreesToGo , 4));
  float currentDegree = targetDEC - degreesToGo;
  // Serial.println("  CurDeg: " + String(currentDegree, 4));
  return currentDegree;
}

void displayStepperPosition() {
  lcdMenu.setCursor(0, 1);
  String disp ;
  if (totalDECMove > 0) {
    float decDist = 100.0 - 100.0 * abs(stepperDEC.distanceToGo()) / totalDECMove;
    // float dec = currentDEC();
    // DayTime dt(dec);
    // int degree = dt.getHours() + (north ? 90 : -90);
    // Serial.println("DEC: " + String(dec, 4) + " DT: " + String(degree) + " " + String(dt.getMinutes()) + " " + String(dt.getSeconds()));
    // sprintf(scratchBuffer, "D: %02d@%02d'%02d\" %d%%", degree, dt.getMinutes(), dt.getSeconds(), (int)decDist);
    sprintf(scratchBuffer, "D: %d%%", (int)decDist);
    disp = String(scratchBuffer);
  }
  else {
    disp = "D:" + String(stepperDEC.currentPosition());
  }
  if (totalRAMove > 0) {
    float raDist = 100.0 - 100.0 * abs(stepperRA.distanceToGo()) / totalRAMove;
    // float ra = currentRA();
    // DayTime dt(ra);
    // dt.addTime(HACorrection);
    // Serial.println("RA: " + String(ra, 4) + " DT: " + dt.ToString());
    // sprintf(scratchBuffer, "R: %02d@%02d'%02d\" %d%%", dt.getHours(), dt.getMinutes(), dt.getSeconds(), (int)raDist);
    sprintf(scratchBuffer, "R: %d%%", (int)raDist);
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

// Move stepper motors to target (non-blocking).
// Returns true if the motor is still running.
// Returns false when the motors are stopped (reached target).
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
