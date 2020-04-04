void BTin();

void loop() {
#ifdef LCD_BUTTON_TEST

  lcdMenu.setCursor(0, 0);
  lcdMenu.printMenu("Key Diagnostic");
  lcd_key = read_LCD_buttons();

  lcdMenu.setCursor(0, 1);
  char buf[128];
  sprintf(buf, "ADC:%4d >", adc_key_in);
  String state = String(buf);
  switch (lcd_key )
  {
    case btnNONE: state += "None"; break;
    case btnSELECT: state += "Select"; break;
    case btnLEFT: state += "Left"; break;
    case btnRIGHT: state += "Right"; break;
    case btnUP: state += "Up"; break;
    case btnDOWN: state += "Down"; break;
  }

  lcdMenu.printMenu(state);

  return;

#endif

  // Give the mount a time slice to do its thing...
  mount.loop();

  lcdMenu.setCursor(0, 1);
  lcd_key = read_LCD_buttons();

  if (inSerialControl) {
    if (lcd_key == btnSELECT) {
      quitSerialOnNextButtonRelease = true;
    }
    else if ((lcd_key == btnNONE) && quitSerialOnNextButtonRelease)  {
      //      handleMeadeQuit("q#");
      quitSerialOnNextButtonRelease = false;
    }
    //    serialLoop();
  }
  else {
    waitForButtonRelease = true;

    // Handle the keys
    if (inStartup) {
      processStartupKeys(lcd_key);
    }
    else {
      switch (lcdMenu.getActive()) {
        case RA_Menu:
          processRAKeys(lcd_key);
          break;
        case DEC_Menu:
          processDECKeys(lcd_key);
          break;
        case POI_Menu:
          processPOIKeys(lcd_key);
          break;
        case Home_Menu:
          processHomeKeys(lcd_key);
          break;
        case HA_Menu:
          processHAKeys(lcd_key);
          break;
        case Heat_Menu:
          processHeatKeys(lcd_key);
          break;
        case Control_Menu:
          processControlKeys(lcd_key);
          break;
        case Calibration_Menu:
          processCalibrationKeys(lcd_key);
          break;
        case Status_Menu:
          processStatusKeys(lcd_key);
          break;
      }
    }

    if (waitForButtonRelease && (lcd_key != btnNONE)) {
      while (read_LCD_buttons() != btnNONE) {
        // Make sure tracker can still run while fiddling with menus....
        mount.loop();
      }
    }

    lcdMenu.setCursor(0, 1);

    if (inStartup) {
      prinStartupMenu();
    }
    else {
      switch (lcdMenu.getActive()) {
        case RA_Menu: printRASubmenu(); break;
        case DEC_Menu: printDECSubmenu(); break;
        case POI_Menu: printPOISubmenu(); break;
        case HA_Menu: printHASubmenu(); break;
        case Home_Menu: printHomeSubmenu(); break;
        case Heat_Menu: printHeatSubmenu(); break;
        case Control_Menu: printControlSubmenu(); break;
        case Calibration_Menu: printCalibrationSubmenu(); break;
        case Status_Menu: printStatusSubmenu(); break;
      }
    }
  }

  BTin();
}
