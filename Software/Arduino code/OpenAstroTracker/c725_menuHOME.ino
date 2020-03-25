void processHomeKeys(int key) {
  switch (key) {
    case btnUP: {
      }
      break;

    case btnDOWN: {
      }
      break;

    case btnSELECT: {
        stepperRA.moveTo(0);
        stepperDEC.moveTo(0);
        moveSteppersToTarget();
      }
      break;

    case btnLEFT: {
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
