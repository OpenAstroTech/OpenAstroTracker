void loop() {

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
