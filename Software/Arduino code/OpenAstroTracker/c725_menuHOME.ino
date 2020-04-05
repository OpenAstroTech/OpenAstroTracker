bool processHomeKeys() {
  byte key;
  if (lcdButtons.keyChanged(key)) {
    switch (key) {
      case btnSELECT: {
          mount.stopSlewing(TRACKING);
          mount.setTargetToHome();
          mount.startSlewingToTarget();
          mount.waitUntilStopped(ALL_DIRECTIONS);
          mount.setHome();
          mount.startSlewing(TRACKING);
        }
        break;

      case btnRIGHT: {
          lcdMenu.setNextActive();
        }
        break;
    }
  }
  
  return true;
}

void printHomeSubmenu() {
  lcdMenu.printMenu(">Go Home");
}
