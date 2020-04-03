void processHomeKeys(int key) {
  switch (key) {
    case btnSELECT: {
        moveToHomePositionsAsync() ;
        while (moveSteppersToTargetAsync()) {
        }
        moveToHomePositionCompleted();
      }
      break;

    case btnRIGHT: {
        lcdMenu.setNextActive();
      }
      break;
  }

}

// Calculate the target RA, DEC and HA adjustments that need to be made and set the motor targets.
void moveToHomePositionsAsync() {
  tracking = 0;
  float trackedSeconds = stepperTRK.currentPosition() / trackingSpeed; // steps/steps/s
  // Serial.println("Pos: " + String(stepperTRK.currentPosition()) + " Secs:" + String(trackedSeconds , 3));

  // In order for RA coordinates to work correctly, we need to
  // offset HATime by elapsed time since last HA set and also
  // adjust RA by the elapsed time and set it to zero.
  RATime.set(0, 0, 0);
  RATime.addSeconds(trackedSeconds);
  HATime.addSeconds(trackedSeconds);
  HACorrection.set(HATime);
  HACorrection.addTime(-h, -m, -s);
  lastHAset = millis();

  // Set DEC to 90degrees
  degreeDEC = minDEC = secDEC = 0;

  // Calculate new RA stepper target (and DEC)
  handleDECandRACalculations();
}

// This must be called when the move to home is completed.
void moveToHomePositionCompleted() {
  // Set TRK stepper position to start counting from here again
  stepperTRK.setCurrentPosition(0);
  stepperRA.setCurrentPosition(0);
  tracking = 1;
}

void printHomeSubmenu() {
  lcdMenu.printMenu(">Go Home");
}
