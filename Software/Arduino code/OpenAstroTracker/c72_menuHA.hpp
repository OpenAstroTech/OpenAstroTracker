#pragma once
#include "EPROMStore.hpp"
#include "configuration_adv.hpp"

#if HEADLESS_CLIENT == 0
#if USE_GPS == 0

bool processHAKeys() {
  byte key;
  bool waitForRelease = false;
  if (lcdButtons.currentState() == btnUP) {
    DayTime ha(mount.HA());
    if (HAselect == 0) ha.addHours(1);
    if (HAselect == 1) ha.addMinutes(1);
    mount.setHA(ha);

    // slow down key repetitions
    mount.delay(200);
  }
  else if (lcdButtons.currentState() == btnDOWN) {
    DayTime ha(mount.HA());
    if (HAselect == 0) ha.addHours(-1);
    if (HAselect == 1) ha.addMinutes(-1);
    mount.setHA(ha);

    // slow down key repetitions
    mount.delay(200);
  }
  else if (lcdButtons.keyChanged(&key)) {
    waitForRelease = true;
    switch (key) {
      case btnLEFT: {
        HAselect = adjustWrap(HAselect, 1, 0, 1);
      }
      break;

      case btnSELECT: {
        EPROMStore::Storage()->update(1, mount.HA().getHours());
        EPROMStore::Storage()->update(2, mount.HA().getMinutes());
        lcdMenu.printMenu("Stored.");
        mount.delay(500);

#if SUPPORT_GUIDED_STARTUP == 1
        if (startupState == StartupWaitForHACompletion) {
          startupState = StartupHAConfirmed;
          inStartup = true;
        }
#endif
        mount.startSlewing(TRACKING);
      }
      break;

      case btnRIGHT: {
#if SUPPORT_GUIDED_STARTUP == 1
        if (startupState != StartupWaitForHACompletion)
#endif
        {
          lcdMenu.setNextActive();
        }
      }
      break;
    }
  }

  return waitForRelease;
 
}

void printHASubmenu() {
    char scratchBuffer[20];
    sprintf(scratchBuffer, " %02dh %02dm", mount.HA().getHours(), mount.HA().getMinutes());
    scratchBuffer[HAselect * 4] = '>';
    lcdMenu.printMenu(scratchBuffer);
}

#endif
#endif
