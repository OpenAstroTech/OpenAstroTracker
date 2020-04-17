#ifndef HEADLESS_CLIENT
byte subGoIndex = 0;

bool processHomeKeys() {
  byte key;
  if (lcdButtons.keyChanged(key)) {
    switch (key) {
      case btnSELECT: {
          if (subGoIndex == 0) {
            mount.goHome(true); // start tracking after home
          } else {
            mount.park();
          }
        }
        break;

      case btnUP:
      case btnDOWN:
      case btnLEFT: {
          subGoIndex = 1 - subGoIndex;
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
  char scratchBuffer[16];
  if (mount.isParked() && (subGoIndex == 1)) {
    lcdMenu.printMenu("Parked...");
  } else {
    strcpy(scratchBuffer, " Home  Park");
    scratchBuffer[subGoIndex * 6] = '>';
    lcdMenu.printMenu(scratchBuffer);
  }
}

#endif
