int infoIndex = 0;
int maxInfoIndex = 3;

void processStatusKeys(int key) {
  switch (key) {

    case btnDOWN: {
        infoIndex = adjustWrap(infoIndex , 1, 0, maxInfoIndex );
      }
      break;
    case btnUP : {
        infoIndex = adjustWrap(infoIndex , -1, 0, maxInfoIndex );
      }
      break;

    case btnSELECT: {
      }
      break;

    case btnRIGHT: {
        lcdMenu.setNextActive();
      }
      break;

    case btnNONE:      {
      }
      break;
  }
}

void printStatusSubmenu() {
  switch (infoIndex)
  {
    case 0: {
        lcdMenu.printMenu("RA:  " + String(stepperRA.currentPosition()));
      }
      break;
    case 1: {
        lcdMenu.printMenu("DEC: " + String(stepperDEC.currentPosition()));
      }
      break;
    case 2: {
        lcdMenu.printMenu("TRK: " + String(stepperTRK.currentPosition()));
      }
      break;
    case 3: {
        lcdMenu.printMenu("Firmw.: " + version);
      }
      break;
  }
}
