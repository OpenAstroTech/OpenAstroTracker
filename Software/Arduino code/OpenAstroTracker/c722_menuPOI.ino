struct PointOfInterest {
  char* pDisplay;
  int hourRA;
  int minRA;
  int secRA;
  int degreeDEC;
  int minDEC;
  int secDEC;
};

PointOfInterest pointOfInterest[] = {
  //    Name (15chars)    RA (hms)     DEC (dms)
  //  012345678901234
  { ">Big Dipper     ", 12, 16, 26,   56, 55,  7 },
  { ">M31 Andromeda  ",  0, 43, 52,   41, 22, 53 },
  { ">M81 Bodes Galxy",  9, 57, 13,   68, 58,  1 },
};

int currentPOI = 0;
int lastPOI = sizeof(pointOfInterest) / sizeof(pointOfInterest[0]) - 1;
int statePOI = 0;
bool poiMoving = false;

void processPOIKeys(int key) {
  switch (key) {
    case btnSELECT: {
        if (lastKey != btnSELECT) {
          stopSteppers();
          PointOfInterest* poi = &pointOfInterest[currentPOI];
          RATime.set(poi->hourRA, poi->minRA, poi->secRA);
          degreeDEC = poi->degreeDEC - (north ? 90 : -90); // iternal DEC degree is 0 at celestial poles
          minDEC = poi->minDEC;
          secDEC = poi->secDEC;

          // Calculate the target stepper positions
          handleDECandRACalculations();

          Serial.println(degreeDEC);

          if (isUnreachable) {
            ShowStatusMessage("Unreachable...");
          }
          else {
            // Calculate total steps needed
            startMoveSteppersToTargetAsync();
            if (!stepperRA.isRunning() && !stepperDEC.isRunning()) {
              ShowStatusMessage("Already there..");
            } else {
              poiMoving = true;
            }
          }
        }
      }
      break;

    case btnLEFT:
    case btnDOWN: {
        currentPOI = adjustWrap(currentPOI, 1, 0, lastPOI);
      }
      break;

    case btnUP: {
        currentPOI = adjustWrap(currentPOI, -1, 0, lastPOI);
      }
      break;

    case btnRIGHT: {
        stopSteppers();
        poiMoving = false;
        lcdMenu.setNextActive();
      }
      break;
  }

  lastKey = key;
  if (poiMoving) {
    moveSteppersToTargetAsync();
    if (!stepperRA.isRunning() && !stepperDEC.isRunning()) {
      poiMoving = false;
    }
  }
}

void printPOISubmenu() {
  if (poiMoving) {
  }
  else {
    lcdMenu.printMenu(pointOfInterest[currentPOI].pDisplay);
  }
}
