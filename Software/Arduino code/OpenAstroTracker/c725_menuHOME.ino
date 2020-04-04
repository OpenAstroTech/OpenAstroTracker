void processHomeKeys(int key) {
  switch (key) {
    case btnSELECT: {
        // TRK stepper is half-stepped so we divide the steps by two to get full steps, which is what RA is stepped at.
        tracking = 0;
        stepperRA.moveTo(0 - stepperTRK.currentPosition() / 2);
        stepperDEC.moveTo(0);

        moveSteppersToTarget();

        // Set TRK stepper position to start counting from here again
        stepperTRK.setCurrentPosition(0);
        stepperRA.setCurrentPosition(0);
        tracking = 1;

        // In order for RA coordinates to work correctly, we need to
        // offset HATime by elapsed time since last HA set.
        unsigned long elapsedMs = millis() - lastHAset;
        HATime.addSeconds(elapsedMs / 1000);
        lastHAset = millis();
      }
      break;

    case btnRIGHT: {
        lcdMenu.setNextActive();
      }
      break;
  }
}

void printHomeSubmenu() {
  lcdMenu.printMenu(">Go Home");
}
