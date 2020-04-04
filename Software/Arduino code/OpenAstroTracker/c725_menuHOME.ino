void processHomeKeys(int key) {
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

void printHomeSubmenu() {
  lcdMenu.printMenu(">Go Home");
}
