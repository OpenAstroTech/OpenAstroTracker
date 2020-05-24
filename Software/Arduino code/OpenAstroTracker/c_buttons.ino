void BTin();

#ifndef HEADLESS_CLIENT

int loopsOfSameKey = 0;
int lastLoopKey = -1;

#ifdef LCD_BUTTON_TEST
byte lastKey = btnNONE;

#endif

void loop() {
  byte lcd_key;
  int adc_key_in;

#ifdef LCD_BUTTON_TEST

  lcdMenu.setCursor(0, 0);
  lcdMenu.printMenu("Key Diagnostic");
  lcd_key = lcdButtons.currentState();
  int key = lcdButtons.currentKey();
  bool changed = lcdButtons.keyChanged(&lastKey);

  adc_key_in = lcdButtons.currentAnalogState();

  lcdMenu.setCursor(0, 1);
  char buf[128];
  sprintf(buf, "A:%4d %d ", adc_key_in, key);
  String state = String(buf);
  switch (lcd_key)
  {
    case btnNONE: state += "None"; break;
    case btnSELECT: state += "Select"; break;
    case btnLEFT: state += "Left"; break;
    case btnRIGHT: state += "Right"; break;
    case btnUP: state += "Up"; break;
    case btnDOWN: state += "Down"; break;
  }

  lcdMenu.printMenu(state);
  if (changed) {
    Serial.println(lastKey);
  }

  return;

#endif


  // Give the mount a time slice to do its thing...
  mount.loop();

  lcdMenu.setCursor(0, 1);

#ifdef SUPPORT_SERIAL_CONTROL
  if (inSerialControl) {
    if (lcdButtons.keyChanged(&lcd_key)) {
      if (lcd_key == btnSELECT) {
        quitSerialOnNextButtonRelease = true;
      }
      else if ((lcd_key == btnNONE) && quitSerialOnNextButtonRelease) {
        MeadeCommandProcessor::instance()->processCommand(":Qq#");
        quitSerialOnNextButtonRelease = false;
      }
    }
    serialLoop();
  }
  else
#endif
  {

    bool waitForButtonRelease = false;

    // Handle the keys
#ifdef SUPPORT_GUIDED_STARTUP
    if (inStartup) {
      waitForButtonRelease = processStartupKeys();
    }
    else
#endif

    {
      switch (lcdMenu.getActive()) {
        case RA_Menu:
        waitForButtonRelease = processRAKeys();
        break;
        case DEC_Menu:
        waitForButtonRelease = processDECKeys();
        break;
#ifdef SUPPORT_POINTS_OF_INTEREST
        case POI_Menu:
        waitForButtonRelease = processPOIKeys();
        break;
#else
        case Home_Menu:
        waitForButtonRelease = processHomeKeys();
        break;
#endif

        case HA_Menu:
        waitForButtonRelease = processHAKeys();
        break;
#ifdef SUPPORT_HEATING
        case Heat_Menu:
        waitForButtonRelease = processHeatKeys();
        break;
#endif

#ifdef SUPPORT_CALIBRATION
        case Calibration_Menu:
        waitForButtonRelease = processCalibrationKeys();
        break;
#endif

#ifdef SUPPORT_MANUAL_CONTROL
        case Control_Menu:
        waitForButtonRelease = processControlKeys();
        break;
#endif

#ifdef SUPPORT_INFO_DISPLAY
        case Status_Menu:
        waitForButtonRelease = processStatusKeys();
        break;
#endif
      }
    }

    if (waitForButtonRelease) {
      if (lcdButtons.currentState() != btnNONE) {
        do {
          byte key;
          if (lcdButtons.keyChanged(&key)) {
            if (key == btnNONE) {
              break;
            }
          }

          // Make sure tracker can still run while fiddling with menus....
          mount.loop();
        } while (true);
      }
    }

    // Input handled, do output
    lcdMenu.setCursor(0, 1);

#ifdef SUPPORT_GUIDED_STARTUP
    if (inStartup) {
      printStartupMenu();
    }
    else
#endif
    {
      if (!inSerialControl) {
        // For some strange reason, a switch statement here causes a crash and reboot....
        int activeMenu = lcdMenu.getActive();
        if (activeMenu == RA_Menu) {
          printRASubmenu();
        }
        else if (activeMenu == DEC_Menu)
        {
          printDECSubmenu();
        }
#ifdef SUPPORT_POINTS_OF_INTEREST
        else if (activeMenu == POI_Menu) {
          printPOISubmenu();
        }
#else
        else if (activeMenu == Home_Menu) {
          printHomeSubmenu();
        }
#endif
        else if (activeMenu == HA_Menu) {
          printHASubmenu();
        }
#ifdef SUPPORT_HEATING
        else if (activeMenu == Heat_Menu) {
          printHeatSubmenu();
        }
#endif
#ifdef SUPPORT_MANUAL_CONTROL
        else if (activeMenu == Control_Menu) {
          printControlSubmenu();
        }
#endif
#ifdef SUPPORT_CALIBRATION
        else if (activeMenu == Calibration_Menu) {
          printCalibrationSubmenu();
        }
#endif

#ifdef SUPPORT_INFO_DISPLAY
        else if (activeMenu == Status_Menu) {
          printStatusSubmenu();
        }
#endif
      }
    }
  }


  BTin();
}
#else

void loop() {
  serialLoop();
  BTin();
}

#endif
