void BTin();
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
  //lcd_key = lcdButtons.currentKey();

  if (inSerialControl) {
    if (lcdButtons.keyChanged(lcd_key)) {
      if (lcd_key == btnSELECT) {
        quitSerialOnNextButtonRelease = true;
      }
      if (lcd_key == btnDOWN) {
        Serial.println("\n\r");
        Serial.println(logString);
      }
      else if ((lcd_key == btnNONE) && quitSerialOnNextButtonRelease)  {
        handleMeadeQuit("q#");
        quitSerialOnNextButtonRelease = false;
      }
      serialLoop();
    }
  }
  else {

    bool waitForButtonRelease = false;

    // Handle the keys
    if (inStartup) {
      waitForButtonRelease = processStartupKeys();
    }
    else {
      switch (lcdMenu.getActive()) {
        case RA_Menu:
          waitForButtonRelease = processRAKeys();
          break;
        case DEC_Menu:
          waitForButtonRelease = processDECKeys();
          break;
        case POI_Menu:
          waitForButtonRelease = processPOIKeys();
          break;
        case Home_Menu:
          waitForButtonRelease = processHomeKeys();
          break;
        case HA_Menu:
          waitForButtonRelease = processHAKeys();
          break;
        case Heat_Menu:
          waitForButtonRelease = processHeatKeys();
          break;
        case Calibration_Menu:
          waitForButtonRelease = processCalibrationKeys();
          break;
        case Control_Menu:
          waitForButtonRelease = processControlKeys();
          break;
        case Status_Menu:
          waitForButtonRelease = processStatusKeys();
          break;
      }
    }

    if (waitForButtonRelease) {
      //Serial.print("Wait:");
      if (lcdButtons.currentKey() != btnNONE) {
        //Serial.println("KeyNotNone:");
        do {
          if (lcdButtons.currentKey() == btnNONE) {
            //Serial.println("KeyIsNone:");
            break;
          }

          // Make sure tracker can still run while fiddling with menus....
          mount.loop();
        }
        while (true);
      }
      //Serial.println("WaitOver:");
    }

    // Input handled, do output
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
