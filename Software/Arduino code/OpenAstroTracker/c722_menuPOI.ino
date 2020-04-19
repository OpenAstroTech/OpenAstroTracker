#ifndef HEADLESS_CLIENT
#ifdef SUPPORT_POINTS_OF_INTEREST
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
  { ">Polaris"        ,  PolarisRAHour, PolarisRAMinute, PolarisRASecond,  89, 21,  2 },
  { ">Big Dipper"     , 12, 16, 26,   56, 55,  7 },
  { ">M31 Andromeda"  ,  0, 43, 52,   41, 22, 53 },
  { ">M42 Orion Nbula",  5, 36, 18,   -5, 22, 44 },
  { ">M51 Whirlpool"  , 13, 30, 45,   47, 05, 21 },
  { ">M63 Sunflower"  , 13, 16, 45,   41, 55, 14 },
  { ">M81 Bodes Galxy",  9, 57, 13,   68, 58,  1 },
  { ">M101 Pinwheel",   14,  3, 56,   54, 15,  0 },
  // Add new items above here, not below.
  { ">Home"           ,  0,  0,  0,   90,  0,  0 },
  { ">Park"           ,  0,  0,  0,   90,  0,  0 },
  // And definitely don't add here.
};

int currentPOI = 0;
int parkPOI = sizeof(pointOfInterest) / sizeof(pointOfInterest[0]) - 1;
byte homePOI = sizeof(pointOfInterest) / sizeof(pointOfInterest[0]) - 2;

bool processPOIKeys() {
  byte key;
  if (lcdButtons.keyChanged(key)) {
    switch (key) {
      case btnSELECT: {
          mount.stopSlewing(ALL_DIRECTIONS);
          if (currentPOI == homePOI) {
            mount.goHome(true);
          } else if (currentPOI == parkPOI) {
            mount.park();
          }
          else {
            PointOfInterest* poi = &pointOfInterest[currentPOI];
            mount.targetRA().set(poi->hourRA, poi->minRA, poi->secRA);
            mount.targetRA().addTime(mount.getHACorrection());
            mount.targetRA().subtractTime(mount.HA());
            mount.targetDEC().set(poi->degreeDEC - (NORTHERN_HEMISPHERE ? 90 : -90), poi->minDEC, poi->secDEC); // internal DEC degree is 0 at celestial poles
            mount.startSlewingToTarget();
          }
        }
        break;

      case btnLEFT:
      case btnDOWN: {
          currentPOI = adjustWrap(currentPOI, 1, 0, parkPOI);
        }
        break;

      case btnUP: {
          currentPOI = adjustWrap(currentPOI, -1, 0, parkPOI);
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
  if (mount.isSlewingIdle()) {
    lcdMenu.printMenu(pointOfInterest[currentPOI].pDisplay);
  }
}
#endif
#endif
