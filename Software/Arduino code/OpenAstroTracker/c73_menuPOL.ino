void processPolarisKeys(int key) {
  switch (key) {
    case btnSELECT: {
        int displaySkip = 100;
        int display = 0;

        hPolarisPosition = 2 - RADisplayTime.getHours();
        mPolarisPosition = 57 - RADisplayTime.getMinutes();
        if (mPolarisPosition < 0) {
          hPolarisPosition -= 1;
          mPolarisPosition += 60;
        }
        if (hPolarisPosition < -6) {
          hPolarisPosition += 24;
          stepperDEC.moveTo(213.4 * 2);
        }
        else stepperDEC.moveTo(-213.4 * 2);

        if (hPolarisPosition > 6) hPolarisPosition -= 12;
        int hPolarisMoveTo = (hPolarisPosition * stepsPerHour + mPolarisPosition * (stepsPerHour / 60)) / 2 ;
        stepperRA.moveTo(-hPolarisMoveTo);

        moveSteppersToTarget();
      }
      break;

    case btnRIGHT: {
        lcdMenu.setNextActive();
      }
      break;
  }
}

void printPolarisSubmenu() {
  lcdMenu.printMenu(">Go to Polaris");
}
