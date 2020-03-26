// Stop the steppers, allowing their decelerations to process (Blocking)
void stopSteppers() {
  // Is it running?
  stepperRA.stop();
  stepperDEC.stop();
  while (stepperRA.isRunning() || stepperDEC.isRunning()) {
    stepperRA.run();
    stepperDEC.run();
    runTracker();
  }
}

// Stop one of the steppers, allowing its decelerations to process and update controlState (Blocking)
// This function is meant for use by the CTRL menu
bool stopStepper(int mask, bool useRA) {
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
      }
    }
    else {
      while (stepperDEC.isRunning()) {
        stepperRA.run();
        stepperDEC.run();
        runTracker();
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

  int displaySkip = 400;  // Update the LCD every 400 iterations (perf issue)
  int display = 0;

  float decTotal = 1.0f * abs(stepperDEC.distanceToGo());
  float raTotal = 1.0f * abs(stepperRA.distanceToGo());
  while (stepperDEC.distanceToGo() != 0 || stepperRA.distanceToGo() != 0) {
    stepperRA.run();
    stepperDEC.run();

    // Run the TRK stepper (same one at slow speed)
    runTracker();

    if (display <= 0) {
      lcdMenu.setCursor(0, 1);
      String disp = "";
      if (decTotal > 0)              {
        float decDist = 100.0 - 100.0 * abs(stepperDEC.distanceToGo()) / decTotal;
        disp = disp + format("DEC:%d%% ", (int)floor(decDist));
      }
      if (raTotal > 0) {
        float raDist = 100.0 - 100.0 * abs(stepperRA.distanceToGo()) / raTotal;
        disp = disp + format("RA:%d%%  ", (int)floor(raDist));
      }
      lcdMenu.printMenu(disp);
      display = displaySkip;
    }
    display--;
  }
}

void startMoveSteppersToTargetAsync() {
  totalDECMove = 1.0f * abs(stepperDEC.distanceToGo());
  totalRAMove = 1.0f * abs(stepperRA.distanceToGo());
}

// Move stepper motors to target (non-blocking)
void moveSteppersToTargetAsync() {
  if (stepperDEC.isRunning() || stepperRA.isRunning()) {
    stepperRA.run();
    stepperDEC.run();

    // Run the TRK stepper (same one at slow speed)
    runTracker();

    if (controlDisplay <= 0) {
      lcdMenu.setCursor(0, 1);
      String disp ;
      if ((totalDECMove == 0) || (totalRAMove == 0)) {
        disp = String(format("D:%l R:%l", stepperDEC.currentPosition(), stepperRA.currentPosition()));
      }
      else {
        disp = "";
        if (totalDECMove > 0)              {
          float decDist = 100.0 - 100.0 * abs(stepperDEC.distanceToGo()) / totalDECMove;
          disp = disp + format("DEC:%d%% ", (int)floor(decDist));
        }
        if (totalRAMove > 0) {
          float raDist = 100.0 - 100.0 * abs(stepperRA.distanceToGo()) / totalRAMove;
          disp = disp + format("RA:%d%%  ", (int)floor(raDist));
        }
      }

      lcdMenu.printMenu(disp);
      controlDisplay  = 250; // Update the LCD every 250 iterations (perf issue)
    }
    controlDisplay --;
  }
}
