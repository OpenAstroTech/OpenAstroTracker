byte infoIndex = 0;
byte maxInfoIndex = 4;
byte stepVsTime = 0;

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

    case btnSELECT:
    case btnLEFT:
      stepVsTime = 1 - stepVsTime;
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
        if (stepVsTime == 0) {
          lcdMenu.printMenu("RA Stpr: " + String(stepperRA.currentPosition()));
        } else {
          lcdMenu.printMenu("RA: " + formatRA(&RADisplayTime));
        }
      }
      break;
    case 1: {
        if (stepVsTime == 0) {
          lcdMenu.printMenu("DEC Stpr:" + String(stepperDEC.currentPosition()));
        } else {
          lcdMenu.printMenu("DEC: " + formatDEC(printdegDEC, minDEC, secDEC));
        }
      }
      break;
    case 2: {
        if (stepVsTime == 0) {
          lcdMenu.printMenu("TRK Stpr:" + String(stepperTRK.currentPosition()));
        } else {
          sprintf(scratchBuffer, "TRK Spd:");
          dtostrf(trackingSpeed, 8, 6, &scratchBuffer[8]);
          lcdMenu.printMenu(scratchBuffer);
        }
      }
      break;
    case 3: {
        long now=millis();
        long msPerDay = 60L * 60 * 24 * 1000;
        int days = (int)(now / msPerDay);
        now -= days * msPerDay;
        DayTime elapsed(now);
        sprintf(scratchBuffer,"Up: %dd %02d:%02d:%02d",days,elapsed.getHours(),elapsed.getMinutes(),elapsed.getSeconds());
        lcdMenu.printMenu(scratchBuffer);
      }
      break;
    case 4: {
        lcdMenu.printMenu("Firmw.: " + version);
      }
      break;
  }
}
