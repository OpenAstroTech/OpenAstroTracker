#pragma once

#if DISPLAY_TYPE > 0
#if SUPPORT_POINTS_OF_INTEREST == 1
struct PointOfInterest
{
  const char *pDisplay;
  byte hourRA;
  byte minRA;
  byte secRA;
  int degreeDEC;
  byte minDEC;
  byte secDEC;
};

// Points of interest are sorted by DEC
// Note that you cannot add any points of interest between 0 and -1 degree Declination (don't ask).
PointOfInterest pointOfInterest[] = {
    // Name (15chars)    RA (hms)     DEC (dms)
    //0123456789012345
    {">Polaris", POLARIS_RA_HOUR, POLARIS_RA_MINUTE, POLARIS_RA_SECOND, 89, 21, 6},
    {">Altair", 19, 51, 45, 8, 55, 15},
    {">Arcturus", 14, 16, 37, 19, 5, 21},
    {">Big Dipper", 12, 16, 26, 56, 55, 7},
    {">Cederblad 214", 0, 5, 46, 67, 16, 45},
    {">Deneb (Cygnus)", 20, 42, 7, 45, 21, 12},
    {">Elephant Trunk", 21, 39, 44, 57, 35, 31},
    {">Heart Nebula", 2, 34, 57, 61, 31, 17},
    {">Lagoon Nebula", 18, 5, 2, -24, 22, 52},
    {">M31 Andromeda", 0, 43, 52, 41, 22, 53},
    {">M33 Triangulum", 1, 35, 02, 30, 46, 5},
    {">M42 Orion Nbula", 5, 36, 18, -5, 22, 44},
    {">M51 Whirlpool", 13, 30, 45, 47, 5, 21},
    {">M63 Sunflower", 13, 16, 45, 41, 55, 14},
    {">M81 Bodes Galxy", 9, 57, 13, 68, 58, 1},
    {">M101 Pinwheel", 14, 3, 56, 54, 15, 0},
    {">Navi", 0, 57, 57, 60, 49, 33},
    {">Pleiades 7Sistr", 3, 48, 15, 24, 10, 54},
    {">Soul Nebula", 2, 52, 47, 60, 30, 56},
    {">Vega", 18, 37, 37, 38, 48, 7},
    {">Veil Nebula", 20, 51, 28, 30, 59, 30},

    // Add new items above here, not below.
    {">Home", 0, 0, 0, 90, 0, 0},
    {">Unpark", 0, 0, 0, 90, 0, 0},
    {">Park", 0, 0, 0, 90, 0, 0},
    // And definitely don't add here.
};

int currentPOI = 0;
int parkPOI = sizeof(pointOfInterest) / sizeof(pointOfInterest[0]) - 1;
int unparkPOI = sizeof(pointOfInterest) / sizeof(pointOfInterest[0]) - 2;
byte homePOI = sizeof(pointOfInterest) / sizeof(pointOfInterest[0]) - 3;

bool processPOIKeys()
{
  byte key;
  bool waitForRelease = false;
  if (lcdButtons.keyChanged(&key))
  {
    waitForRelease = true;
    switch (key)
    {
    case btnSELECT:
    {
      mount.stopSlewing(ALL_DIRECTIONS);
      if (currentPOI == homePOI)
      {
        mount.goHome();
      }
      else if (currentPOI == parkPOI)
      {
        mount.park();
      }
      else if (currentPOI == unparkPOI)
      {
        mount.startSlewing(TRACKING);
      }
      else
      {
        PointOfInterest *poi = &pointOfInterest[currentPOI];
        LOGV5(DEBUG_INFO, F("POI: Selected %s.  RA: %d %d %d"), poi->pDisplay, poi->hourRA, poi->minRA, poi->secRA);
        LOGV5(DEBUG_INFO, F("POI: Selected %s. DEC: %d %d %d"), poi->pDisplay, poi->degreeDEC, poi->minDEC, poi->secDEC);
        long targetSeconds = (60L * abs(poi->degreeDEC) + poi->minDEC) * 60L + poi->secDEC;
        targetSeconds *= (poi->degreeDEC < 0 ? -1 : 1);
        mount.targetRA().set(poi->hourRA, poi->minRA, poi->secRA);
        mount.targetDEC() = Declination::FromSeconds(targetSeconds);
        LOGV3(DEBUG_INFO, F("POI: mount target RA  is %s. %ls"), mount.targetRA().ToString(), targetSeconds);
        LOGV3(DEBUG_INFO, F("POI: mount target DEC is %s. %ls"), mount.targetDEC().ToString(), mount.targetDEC().getTotalSeconds());
        mount.startSlewingToTarget();
      }
    }
    break;

    case btnLEFT:
    case btnDOWN:
    {
      currentPOI = adjustWrap(currentPOI, 1, 0, parkPOI);
    }
    break;

    case btnUP:
    {
      currentPOI = adjustWrap(currentPOI, -1, 0, parkPOI);
    }
    break;

    case btnRIGHT:
    {
      lcdMenu.setNextActive();
    }
    break;
    }
  }

  return waitForRelease;
}

void printPOISubmenu()
{
  if (mount.isSlewingIdle())
  {
    lcdMenu.printMenu(pointOfInterest[currentPOI].pDisplay);
  }
}
#endif
#endif
