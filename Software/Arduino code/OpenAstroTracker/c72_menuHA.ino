void processHAKeys(int key) {
  switch (key) {
    case btnUP: {
        DayTime ha(mount.HA());
        if (HAselect == 0) ha.addHours(1);
        if (HAselect == 1) ha.addMinutes(1);
        mount.setHA(ha);

        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnDOWN: {
        DayTime ha(mount.HA());
        if (HAselect == 0) ha.addHours(-1);
        if (HAselect == 1) ha.addMinutes(-1);
        mount.setHA(ha);

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
        EEPROM.update(1, mount.HA().getHours());
        EEPROM.update(2, mount.HA().getMinutes());

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
  char scratchBuffer[20];
  sprintf(scratchBuffer, " %02dh %02dm", mount.HA().getHours(), mount.HA().getMinutes());
  scratchBuffer[HAselect * 4] = '>';
  lcdMenu.printMenu(scratchBuffer);
}
