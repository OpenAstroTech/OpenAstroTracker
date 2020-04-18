#ifndef HEADLESS_CLIENT
bool processHAKeys() {
  byte key;
  bool waitForRelease = true;
  if (lcdButtons.currentState() == btnUP)  {
    DayTime ha(mount.HA());
    if (HAselect == 0) ha.addHours(1);
    if (HAselect == 1) ha.addMinutes(1);
    mount.setHA(ha);

    // slow down key repetitions
    mount.delay(200);
    waitForRelease = false;
  }
  else if (lcdButtons.currentState() == btnDOWN)  {
    DayTime ha(mount.HA());
    if (HAselect == 0) ha.addHours(-1);
    if (HAselect == 1) ha.addMinutes(-1);
    mount.setHA(ha);

    // slow down key repetitions
    mount.delay(200);
    waitForRelease = false;
  }
  else if (lcdButtons.keyChanged(key)) {
    switch (key) {

      case btnLEFT: {
          HAselect = adjustWrap(HAselect, 1, 0, 1);
        }
        break;

      case btnSELECT: {
          EEPROM.update(1, mount.HA().getHours());
          EEPROM.update(2, mount.HA().getMinutes());
          lcdMenu.printMenu("Stored.");
          mount.delay(500);

#ifdef SUPPORT_GUIDED_STARTUP
          if (startupState == StartupWaitForHACompletion) {
            startupState = StartupHAConfirmed;
            inStartup = true;
          }
#endif
        }
        break;

      case btnRIGHT: {
#ifdef SUPPORT_GUIDED_STARTUP
          if (startupState != StartupWaitForHACompletion)
#endif
          {
            lcdMenu.setNextActive();
          }
        }
        break;
    }
  }

  return waitForRelease;
}

void printHASubmenu() {
  char scratchBuffer[20];
  sprintf(scratchBuffer, " %02dh %02dm", mount.HA().getHours(), mount.HA().getMinutes());
  scratchBuffer[HAselect * 4] = '>';
  lcdMenu.printMenu(scratchBuffer);
}
#endif
