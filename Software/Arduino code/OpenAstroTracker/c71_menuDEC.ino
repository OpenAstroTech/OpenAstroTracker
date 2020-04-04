
void processDECKeys(int key) {

  switch (key) {
    case btnUP: {
        if (DECselect == 0) mount.targetDEC().addDegrees(1);
        if (DECselect == 1) mount.targetDEC().addMinutes(1);
        if (DECselect == 2) mount.targetDEC().addSeconds(1);
        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnDOWN: {
        if (DECselect == 0) mount.targetDEC().addDegrees(-1);
        if (DECselect == 1) mount.targetDEC().addMinutes(-1);
        if (DECselect == 2) mount.targetDEC().addSeconds(-1);
        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnLEFT: {
        DECselect = adjustWrap(DECselect, 1, 0, 2);
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

void printDECSubmenu() {
  if (mount.isSlewingIdle()) {
    lcdMenu.printMenu(mount.DECString(LCDMENU_STRING | TARGET_STRING, DECselect));
  }
}
