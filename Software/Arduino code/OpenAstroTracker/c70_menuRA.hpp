#pragma once
/*
#if HEADLESS_CLIENT == 0
bool showTargetRA = true;
bool processRAKeys() {
  byte key;
  bool waitForRelease = false;
  if (lcdButtons.currentState() == btnUP) {
    if (RAselect == 0) { mount.targetRA().addHours(1); showTargetRA = true; }
    if (RAselect == 1) { mount.targetRA().addMinutes(1); showTargetRA = true; }
    if (RAselect == 2) { mount.targetRA().addSeconds(1); showTargetRA = true; }
    if (RAselect == 3) { showTargetRA = !showTargetRA; waitForRelease=true; }

    // slow down key repetitions
    mount.delay(200);
  }
  else if (lcdButtons.currentState() == btnDOWN) {
    if (RAselect == 0) { mount.targetRA().addHours(-1); showTargetRA = true; }
    if (RAselect == 1) { mount.targetRA().addMinutes(-1); showTargetRA = true; }
    if (RAselect == 2) { mount.targetRA().addSeconds(-1); showTargetRA = true; }
    if (RAselect == 3) { showTargetRA = !showTargetRA; waitForRelease=true; }

    // slow down key repetitions
    mount.delay(200);
  }
  else if (lcdButtons.keyChanged(&key)) {
    waitForRelease = true;
    switch (key)
    {
      case btnLEFT: {
        RAselect = adjustWrap(RAselect, 1, 0, 3);
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
    String ra = mount.RAString(LCDMENU_STRING | (showTargetRA ? TARGET_STRING : CURRENT_STRING), RAselect).substring(0,12);
    ra += (RAselect == 3) ? ">" : " ";
    ra += showTargetRA ? "Ta" : "Cu";
    lcdMenu.printMenu(ra);
  }
}
#endif
*/