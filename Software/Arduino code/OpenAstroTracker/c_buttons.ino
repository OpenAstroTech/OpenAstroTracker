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



  lcd_key = read_LCD_buttons();

  trackingspeed = ((((335.1417 / 288.0) * RevSteps) / 3590)) - 1 + float(speedcalibration);
  stepperTRK.setSpeed(trackingspeed);

  waitForButtonRelease = true;
  lcd.setCursor(0, 1);

  // Handle the keys
  if (inStartup) {
    processStartupKeys(lcd_key);
  }
  else {
    switch (lcdMenu.getActive()) {
      case RA_Menu:
        handleDECandRACalculations();
        processRAKeys(lcd_key);
        break;
      case DEC_Menu:
        handleDECandRACalculations();
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
      case Polaris_Menu:
        processPolarisKeys(lcd_key);
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
    }
  }

  if (waitForButtonRelease && (lcd_key != btnNONE)) {
    while (read_LCD_buttons() != btnNONE) {
      // Make sure tracker can still run while fiddling with menus....
      runTracker();
    }
  }

  runTracker();
  doCalculations();

  if (!pcControl) {

    lcd.setCursor(0, 0);

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
        case Polaris_Menu: printPolarisSubmenu(); break;
        case Heat_Menu: printHeatSubmenu(); break;
        case Control_Menu: printControlSubmenu(); break;
        case Calibration_Menu: printCalibrationSubmenu(); break;
      }
    }

    //tracking menu  ----------------------
    /*if (lcdMenu.getActive() == TRK_Menu) {
      lcd.print("Tracking ON OFF");
      lcd.print("         ");
      }*/
  } else {
    moveSteppersToTargetAsync();
  }

  BTin();
}
