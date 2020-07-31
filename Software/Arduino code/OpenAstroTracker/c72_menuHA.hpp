#pragma once
#include "EPROMStore.hpp"

#if HEADLESS_CLIENT == 0


bool processHAKeys() {
#if USE_GPS == 0
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
        mount.setHome();

#if SUPPORT_GUIDED_STARTUP == 1
        if (startupState == StartupWaitForHACompletion) {
          startupState = StartupHAConfirmed;
          inStartup = true;
        }
#endif
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
  
#else // USE_GPS = 1
byte key;
bool waitForRelease = false;
if (startupState == StartupWaitForHACompletion) {
while (GPS_SERIAL_PORT.available()) {  
if (gps.encode(GPS_SERIAL_PORT.read())) {
  if (gps.location.lng() != 0) {

    lcdMenu.printMenu("Acquired Pos.");
    
    int hHAfromLST = Sidereal::calculateByGPS(&gps).getHours() - PolarisRAHour;
    int mHAfromLST = Sidereal::calculateByGPS(&gps).getMinutes() - PolarisRAMinute;
    mount.setHA(DayTime(hHAfromLST, mHAfromLST, 0));
    
    EPROMStore::Storage()->update(1, mount.HA().getHours());
    EPROMStore::Storage()->update(2, mount.HA().getMinutes());
    mount.delay(500);
    mount.setHome();
    
    startupState = StartupHAConfirmed;
    inStartup = true;
  }
}
}
}
if (lcdButtons.keyChanged(&key)) {
  switch (key) {
  case btnRIGHT: {
    lcdMenu.setNextActive();
  }
}
}

#endif
}

void printHASubmenu() {
  #if USE_GPS == 0
  char scratchBuffer[20];
  sprintf(scratchBuffer, " %02dh %02dm", mount.HA().getHours(), mount.HA().getMinutes());
  scratchBuffer[HAselect * 4] = '>';
  lcdMenu.printMenu(scratchBuffer);
  #else
  if (startupState == StartupWaitForHACompletion) {
    char satbuffer[20];
    sprintf(satbuffer, "Found %02d Sats", gps.satellites.value());
    lcdMenu.printMenu(satbuffer);
    //lcd.print("Found ");
    //lcd.print(gps.satellites.value());
    //lcd.print(" Sats");
  }
  else {
    char scratchBuffer[20];
    sprintf(scratchBuffer, " %02dh %02dm", mount.HA().getHours(), mount.HA().getMinutes());
    lcdMenu.printMenu(scratchBuffer);
  }
  #endif
}
#endif
