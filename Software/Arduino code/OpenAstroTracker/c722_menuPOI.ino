struct PointOfInterest {
  char* pDisplay;
  byte hourRA;
  byte minRA;
  byte secRA;
  byte degreeDEC;
  byte minDEC;
  byte secDEC;
};

PointOfInterest pointOfInterest[] = {
  //    Name (15chars)    RA (hms)     DEC (dms)
  //  012345678901234
  { ">Polaris        ",  2, 57, 56,   89, 21,  2 },
  { ">Big Dipper     ", 12, 16, 26,   56, 55,  7 },
  { ">M31 Andromeda  ",  0, 43, 52,   41, 22, 53 },
  { ">M81 Bodes Galxy",  9, 57, 13,   68, 58,  1 },
};

int currentPOI = 0;
int lastPOI = sizeof(pointOfInterest) / sizeof(pointOfInterest[0]) - 1;

bool processPOIKeys() {
  byte key;
  if (lcdButtons.keyChanged(key)) {
    switch (key) {
      case btnSELECT: {
          mount.stopSlewing(ALL_DIRECTIONS);
          PointOfInterest* poi = &pointOfInterest[currentPOI];
          mount.targetRA().set(poi->hourRA, poi->minRA, poi->secRA);
          mount.targetRA().addTime(mount.getHACorrection());
          mount.targetRA().subtractTime(mount.HA());
          mount.targetDEC().set(poi->degreeDEC - (NORTHERN_HEMISPHERE ? 90 : -90), poi->minDEC, poi->secDEC); // internal DEC degree is 0 at celestial poles
          mount.startSlewingToTarget();
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
          lcdMenu.setNextActive();
        }
        break;
    }
  }

  return true;
}

void printPOISubmenu() {
  //Serial.println("POI: "+String(mount.isSlewingIdle()));
  if (mount.isSlewingIdle()) {
    lcdMenu.printMenu(pointOfInterest[currentPOI].pDisplay);
  }
}
