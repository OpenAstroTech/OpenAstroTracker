void processHomeKeys(int key) {
  switch (key) {
    case btnSELECT: {
        if ((stepperRA.currentPosition() == 0) && (stepperDEC.currentPosition()==0)) {
          lcdMenu.setCursor(0, 1);
          lcdMenu.printMenu("Already Home...");
          long now = millis();
          while (millis() - now < 750) {
            runTracker();
          }
        }
        
        stepperRA.moveTo(0);
        stepperDEC.moveTo(0);
        moveSteppersToTarget();
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
