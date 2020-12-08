#pragma once

#if DISPLAY_TYPE > 0

#if SUPPORT_INFO_DISPLAY == 1
byte infoIndex = 0;
byte maxInfoIndex = 13;
byte subIndex = 0;
unsigned long lastInfoUpdate = 0;

bool processStatusKeys()
{
  byte key;
  bool waitForRelease = false;
  if (lcdButtons.keyChanged(&key))
  {
    waitForRelease = true;
    lastInfoUpdate = 0; // Force immediate display
    switch (key)
    {
    case btnDOWN:
    {
      infoIndex = adjustWrap(infoIndex, 1, 0, maxInfoIndex);
    }
    break;

    case btnUP:
    {
      infoIndex = adjustWrap(infoIndex, -1, 0, maxInfoIndex);
    }
    break;

    case btnSELECT:
    case btnLEFT:
    {
      subIndex = adjustWrap(subIndex, 1, 0, 1 + (infoIndex < 2 ? 1 : 0));
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

void printStatusSubmenu()
{
  if (millis() - lastInfoUpdate > DISPLAY_UPDATE_TIME)
  {
    char scratchBuffer[20];
    switch (infoIndex)
    {
    case 0:
    {
      if (subIndex == 0)
      {
        lcdMenu.printMenu("RA Stpr: " + String(mount.getCurrentStepperPosition(WEST)));
      }
      else if (subIndex == 1)
      {
        lcdMenu.printMenu("RTrg: " + mount.RAString(LCD_STRING | TARGET_STRING));
      }
      else
      {
        lcdMenu.printMenu("RCur: " + mount.RAString(LCD_STRING | CURRENT_STRING));
      }
    }
    break;

    case 1:
    {
      if (subIndex == 0)
      {
        lcdMenu.printMenu("DEC Stpr:" + String(mount.getCurrentStepperPosition(NORTH)));
      }
      else if (subIndex == 1)
      {
        lcdMenu.printMenu("DTrg: " + mount.DECString(LCD_STRING | TARGET_STRING));
      }
      else
      {
        lcdMenu.printMenu("DCur: " + mount.DECString(LCD_STRING | CURRENT_STRING));
      }
    }
    break;

    case 2:
    {
      if (subIndex == 0)
      {
        lcdMenu.printMenu("TRK Stpr:" + String(mount.getCurrentStepperPosition(TRACKING)));
      }
      else
      {
        sprintf(scratchBuffer, "TRK Spd:");
        dtostrf(mount.getSpeed(TRACKING), 8, 6, &scratchBuffer[8]);
        lcdMenu.printMenu(scratchBuffer);
      }
    }
    break;

    case 3:
    {
      float lat = fabs(mount.latitude().getTotalHours());
      float lng = fabs(mount.longitude().getTotalHours());
      const char dirLat = (mount.latitude().getTotalHours() < 0) ? 'S' : 'N';
      const char dirLong = (mount.longitude().getTotalHours() < 0) ? 'W' : 'E';
      sprintf(scratchBuffer, "Loc %s%c %s%c", String(lat, 1).c_str(), dirLat, String(lng, 1).c_str(), dirLong);
      lcdMenu.printMenu(scratchBuffer);
    }
    break;

    case 4:
    {
#if USE_GYRO_LEVEL == 1
      int celsius = (int)round(Gyro::getCurrentTemperature());
      int fahrenheit = (int)round(32.0 + 9.0 * Gyro::getCurrentTemperature() / 5.0);

      sprintf(scratchBuffer, "Temp: %d@C %d@F", celsius, fahrenheit);
      lcdMenu.printMenu(scratchBuffer);
#else
      infoIndex++;
#endif
    }
    break;

    case 5:
    {
      long lowerLimit, upperLimit;
      mount.getDecLimitPositions(lowerLimit, upperLimit);
      lcdMenu.printMenu("DEC Lo: " + String(lowerLimit));
    }
    break;

    case 6:
    {
      long lowerLimit, upperLimit;
      mount.getDecLimitPositions(lowerLimit, upperLimit);
      lcdMenu.printMenu("DEC Up: " + String(upperLimit));
    }
    break;

    case 7:
    {
      sprintf(scratchBuffer, "MemAvail: %d", freeMemory());
      lcdMenu.printMenu(scratchBuffer);
    }
    break;

    case 8:
    {
      long now = millis();
      long msPerDay = 60L * 60 * 24 * 1000;
      int days = (int)(now / msPerDay);
      now -= days * msPerDay;
      DayTime elapsed(1.0 * now / (1000.0 * 3600.0));
      sprintf(scratchBuffer, "Up: %dd %02d:%02d:%02d", days, elapsed.getHours(), elapsed.getMinutes(), elapsed.getSeconds());
      lcdMenu.printMenu(scratchBuffer);
    }
    break;

    case 9:
    {
        LocalDate date = mount.localDate();
        sprintf(scratchBuffer, "Date: %04d-%02d-%02d", date.year, date.month, date.day );
        lcdMenu.printMenu(scratchBuffer);
    }
    break;

    case 10:
    {
        DayTime drvUtc = mount.utcTime();
        sprintf(scratchBuffer, "UTC: %02d:%02d:%02d", drvUtc.getHours(), drvUtc.getMinutes(), drvUtc.getSeconds() );
        lcdMenu.printMenu(scratchBuffer);
    }
    break;

    case 11:
    {
        DayTime lst = mount.calculateLst();
        sprintf(scratchBuffer, "LST: %02d:%02d:%02d", lst.getHours(), lst.getMinutes(), lst.getSeconds() );
        lcdMenu.printMenu(scratchBuffer);
    }
    break;

    case 12:
    {
        DayTime ha = mount.calculateHa();
        sprintf(scratchBuffer, "HA: %02d:%02d:%02d", ha.getHours(), ha.getMinutes(), ha.getSeconds() );
        lcdMenu.printMenu(scratchBuffer);
    }
    break;


    case 13:
    {
      lcdMenu.printMenu("Firmw.: " + String(VERSION));
    }
    break;
    }
    lastInfoUpdate = millis();
  }
}

#endif

#endif
