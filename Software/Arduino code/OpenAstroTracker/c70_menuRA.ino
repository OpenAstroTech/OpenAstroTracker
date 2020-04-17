#ifndef HEADLESS_CLIENT
bool processRAKeys() {
  byte key;
  bool waitForRelease = true;
  if (lcdButtons.currentState() == btnUP)  {
    if (RAselect == 0) mount.targetRA().addHours(1);
    if (RAselect == 1) mount.targetRA().addMinutes(1);
    if (RAselect == 2) mount.targetRA().addSeconds(1);

    // slow down key repetitions
    mount.delay(200);
    waitForRelease = false;
  } else if (lcdButtons.currentState() == btnDOWN)  {
    if (RAselect == 0) mount.targetRA().addHours(-1);
    if (RAselect == 1) mount.targetRA().addMinutes(-1);
    if (RAselect == 2) mount.targetRA().addSeconds(-1);

    // slow down key repetitions
    mount.delay(200);
    waitForRelease = false;
  }
  else if (lcdButtons.keyChanged(key)) {
    switch (key)
    {
      case btnLEFT: {
          RAselect = adjustWrap(RAselect, 1, 0, 2);
        }
        break;

      case btnSELECT: {
          if (mount.isSlewingRAorDEC()) {
            mount.stopSlewing(ALL_DIRECTIONS);
            mount.waitUntilStopped(ALL_DIRECTIONS);
          }

          mount.startSlewingToTarget();
        }
        break;

      case btnRIGHT: {
          lcdMenu.setNextActive();
        }
        break;
    }
  }
  
  return waitForRelease;
}

void printRASubmenu() {
  if (mount.isSlewingIdle()) {
    lcdMenu.printMenu(mount.RAString(LCDMENU_STRING | TARGET_STRING, RAselect));
  }
}
#endif
