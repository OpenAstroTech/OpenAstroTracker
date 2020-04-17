void BTin();

#ifndef HEADLESS_CLIENT

int loopsOfSameKey = 0;
int lastLoopKey = -1;

void loop() {
  byte lcd_key;
  int adc_key_in;

#ifdef LCD_BUTTON_TEST

  lcdMenu.setCursor(0, 0);
  lcdMenu.printMenu("Key Diagnostic");
  int lcd_key = lcdButtons.currentState();
  adc_key_in = lcdButtons.currentAnalogState();

  lcdMenu.setCursor(0, 1);
  char buf[128];
  sprintf(buf, "ADC:%4d >", adc_key_in);
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
  if (lcd_key != lastKey) {
    Serial.println(state);
    lastKey = lcd_key;
  }

  return;

#endif

  // Give the mount a time slice to do its thing...
  mount.loop();

  lcdMenu.setCursor(0, 1);

#ifdef SUPPORT_SERIAL_CONTROL
  if (inSerialControl) {
    if (lcdButtons.keyChanged(lcd_key)) {
      if (lcd_key == btnSELECT) {
        quitSerialOnNextButtonRelease = true;
      }
      else if ((lcd_key == btnNONE) && quitSerialOnNextButtonRelease)  {
        handleMeadeQuit("q#");
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
        case Calibration_Menu:
          waitForButtonRelease = processCalibrationKeys();
          break;

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
      if (lcdButtons.currentKey() != btnNONE) {
        do {
          if (lcdButtons.currentKey() == btnNONE) {
            break;
          }

          // Make sure tracker can still run while fiddling with menus....
          mount.loop();
        }
        while (true);
      }
    }

    // Input handled, do output
    lcdMenu.setCursor(0, 1);

#ifdef SUPPORT_GUIDED_STARTUP
    if (inStartup) {
      prinStartupMenu();
    }
    else
#endif
    {
      if (!inSerialControl) {
        switch (lcdMenu.getActive()) {
          case RA_Menu: printRASubmenu(); break;
          case DEC_Menu: printDECSubmenu(); break;
#ifdef SUPPORT_POINTS_OF_INTEREST
          case POI_Menu: printPOISubmenu(); break;
#else
          case Home_Menu: printHomeSubmenu(); break;
#endif
          case HA_Menu: printHASubmenu(); break;
#ifdef SUPPORT_HEATING
          case Heat_Menu: printHeatSubmenu(); break;
#endif
#ifdef SUPPORT_MANUAL_CONTROL
          case Control_Menu: printControlSubmenu(); break;
#endif
          case Calibration_Menu: printCalibrationSubmenu(); break;

#ifdef SUPPORT_INFO_DISPLAY
          case Status_Menu: printStatusSubmenu(); break;
#endif
        }
      }
    }
  }

  BTin();
}
#else

void loop(){
  serialLoop();
  BTin();
}

#endif
