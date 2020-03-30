void processHAKeys(int key) {
  switch (key) {
    case btnUP: {
        if (HAselect == 0) HATime.addHours(1);
        if (HAselect == 1) HATime.addMinutes(1);
        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnDOWN: {
        if (HAselect == 0) HATime.addHours(-1);
        if (HAselect == 1) HATime.addMinutes(-1);

        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnLEFT: {
        HAselect = adjustWrap(HAselect, 1, 0, 1);
      }
      break;

    case btnSELECT:
    case btnRIGHT: {
        EEPROM.update(1, HATime.getHours());
        EEPROM.update(2, HATime.getMinutes());
        HACorrection.set(HATime);
        HACorrection.addTime(-h, -m, -s);

        lastHAset = millis();

        if (startupState == StartupWaitForHACompletion) {
          startupState = StartupHAConfirmed;
          inStartup = true;
        }
        else {
          lcdMenu.setNextActive();
        }
      }
      break;
  }
}

void printHASubmenu() {
  if (HAselect == 0) {
    lcdMenu.printMenuArg(">%dh %dm", HATime.getHours(), HATime.getMinutes());
  }
  if (HAselect == 1) {
    lcdMenu.printMenuArg(" %dh>%dm", HATime.getHours(), HATime.getMinutes());
  }
}
