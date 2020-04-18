#ifndef HEADLESS_CLIENT

#ifdef SUPPORT_INFO_DISPLAY
byte infoIndex = 0;
byte maxInfoIndex = 4;
byte subIndex = 0;

bool processStatusKeys() {
  byte key;
  if (lcdButtons.keyChanged(key)) {
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
        subIndex = adjustWrap(subIndex, 1, 0, 1 + (infoIndex < 2 ? 1 : 0));
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
  return true;
}

void printStatusSubmenu() {
  char scratchBuffer[20];
  switch (infoIndex)
  {
    case 0: {
        if (subIndex  == 0) {
          lcdMenu.printMenu("RA Stpr: " + String(mount.getCurrentStepperPosition(WEST)));
        } else if (subIndex  == 1) {
          lcdMenu.printMenu("RTrg: " + mount.RAString(LCD_STRING | TARGET_STRING));
        } else {
          lcdMenu.printMenu("RCur: " + mount.RAString(LCD_STRING | CURRENT_STRING));
        }
      }
      break;
    case 1: {
        if (subIndex  == 0) {
          lcdMenu.printMenu("DEC Stpr:" + String(mount.getCurrentStepperPosition(NORTH)));
        } else if (subIndex  == 1) {
          lcdMenu.printMenu("DTrg: " + mount.DECString(LCD_STRING | TARGET_STRING));
        } else {
          lcdMenu.printMenu("DCur: " + mount.DECString(LCD_STRING | CURRENT_STRING));
        }
      }
      break;
    case 2: {
        if (subIndex  == 0) {
          lcdMenu.printMenu("TRK Stepr:" + String(mount.getCurrentStepperPosition(TRACKING)));
        } else {
          sprintf(scratchBuffer, "TRK Spd:");
          dtostrf(mount.getSpeed(TRACKING), 8, 6, &scratchBuffer[8]);
          lcdMenu.printMenu(scratchBuffer);
        }
      }
      break;
    case 3: {
        long now = millis();
        long msPerDay = 60L * 60 * 24 * 1000;
        int days = (int)(now / msPerDay);
        now -= days * msPerDay;
        DayTime elapsed(now);
        sprintf(scratchBuffer, "Up: %dd %02d:%02d:%02d", days, elapsed.getHours(), elapsed.getMinutes(), elapsed.getSeconds());
        lcdMenu.printMenu(scratchBuffer);
      }
      break;
    case 4: {
        lcdMenu.printMenu("Firmw.: " + version);
      }
      break;
  }
}

#endif

#endif
