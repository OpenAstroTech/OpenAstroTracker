#pragma once
/*
#if HEADLESS_CLIENT == 0
bool showTargetDEC = true;
bool processDECKeys() {
  byte key;
  bool waitForRelease = false;
  if (lcdButtons.currentState() == btnUP) {
    if (DECselect == 0) { mount.targetDEC().addDegrees(1); showTargetDEC = true; }
    if (DECselect == 1) { mount.targetDEC().addMinutes(1); showTargetDEC = true; }
    if (DECselect == 2) { mount.targetDEC().addSeconds(1); showTargetDEC = true; }
    if (DECselect == 3) { showTargetDEC = !showTargetDEC; waitForRelease=true; }
    // slow down key repetitions
    mount.delay(200);
  }
  else if (lcdButtons.currentState() == btnDOWN) {
    if (DECselect == 0) { mount.targetDEC().addDegrees(-1); showTargetDEC = true; }
    if (DECselect == 1) { mount.targetDEC().addMinutes(-1); showTargetDEC = true; }
    if (DECselect == 2) { mount.targetDEC().addSeconds(-1); showTargetDEC = true; }
    if (DECselect == 3) { showTargetDEC = !showTargetDEC; waitForRelease=true; }
    // slow down key repetitions
    mount.delay(200);
  }
  else if (lcdButtons.keyChanged(&key)) {
    waitForRelease = true;
    switch (key)
    {
      case btnLEFT: {
        DECselect = adjustWrap(DECselect, 1, 0, 3);
      }
      break;

      case btnSELECT: {
        if (mount.isSlewingRAorDEC()) {
          mount.stopSlewing(ALL_DIRECTIONS);
        }
        else {
          mount.startSlewingToTarget();
        }
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

void printDECSubmenu() {
  if (mount.isSlewingIdle()) {
    String dec = mount.DECString(LCDMENU_STRING | (showTargetDEC ? TARGET_STRING : CURRENT_STRING), DECselect).substring(0,13);
    dec += (DECselect == 3) ? ">" : " ";
    dec += showTargetDEC? "Ta" : "Cu";
    lcdMenu.printMenu(dec);
  }
}

#endif
*/