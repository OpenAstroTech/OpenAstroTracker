void loop() {
  bool waitForButtonRelease = false;

  lcd_key = read_LCD_buttons();

  trackingspeed = ((((335.1417 / 288.0) * RevSteps) / 3590)) - 1 + float(speedcalibration);

  handleDECandRACalculations();

  // Handle the keys
  switch (lcd_key) {
    case btnUP: {
        if (lcdMenu.getActive() == RA_Menu) {
          if (RAselect == 0) RATime.addHours(1);
          if (RAselect == 1) RATime.addMinutes(1);
          if (RAselect == 2) RATime.addSeconds(1);
          hourRA = RATime.getHours();
          minRA = RATime.getMinutes();
        }
        if (lcdMenu.getActive() == DEC_Menu) {
          if (DECselect == 0) degreeDEC = north ? adjustClamp(degreeDEC, 1, -180, 0) : adjustClamp(degreeDEC, 1, 0, 180);
          if (DECselect == 1) minDEC = adjustMinute(minDEC, 1);
          if (DECselect == 2) secDEC = adjustSecond(secDEC, 1);
        }
        if (lcdMenu.getActive() == HA_Menu) {
          if (HAselect == 0) HATime.addHours(1);
          if (HAselect == 1) HATime.addMinutes(1);
        }
        if (lcdMenu.getActive() == Calibration_Menu) {
          inputcal += 1;  //0.0001;
        }

        if (lcdMenu.getActive() == Control_Menu) {
          stepperDEC.move(200);
        }
        else {
          delay(150);
        }
        break;
      }

    case btnDOWN: {
        if (lcdMenu.getActive() == RA_Menu) {
          if (RAselect == 0) RATime.addHours(-1);
          if (RAselect == 1) RATime.addMinutes(-1);
          if (RAselect == 2) RATime.addSeconds(-1);

          if (RAselect == 0) hourRA -= 1;
          if (RAselect == 1) minRA -= 1;
          if (RAselect == 2) secRA -= 1;
        }
        if (lcdMenu.getActive() == DEC_Menu) {
          if (DECselect == 0) degreeDEC = north ? adjustClamp(degreeDEC, -1, -180, 0) : adjustClamp(degreeDEC, -1, 0, 180);
          if (DECselect == 1) minDEC = adjustMinute(minDEC, -1);
          if (DECselect == 2) secDEC = adjustSecond(secDEC, -1);
        }
        if (lcdMenu.getActive() == HA_Menu) {
          if (HAselect == 0) HATime.addHours(-1);
          if (HAselect == 1) HATime.addMinutes(-1);

          if (HAselect == 0) hourHA -= 1;
          if (HAselect == 1) minHA -= 1;
          if (hourHA < 0) hourHA += 24;
          if (minHA < 0) minHA += 60;
        }

        if (lcdMenu.getActive() == Calibration_Menu) {
          inputcal -= 1 ; //0.0001;
        }

        if (lcdMenu.getActive() == Control_Menu) {
          stepperDEC.move(-200);
        }
        else {
          delay(150);
        }
        break;
      }

    case btnSELECT: {
        /*stepperRA.moveTo(-moveRA);
          stepperDEC.moveTo(moveDEC);*/
        waitForButtonRelease = true;
        int displaySkip = 100;
        int display = 0;

        // Move stepper motors to target if menu mode is RA or DEC
        if (lcdMenu.getActive() < HA_Menu) {
          float decTotal = 1.0f * abs(stepperDEC.distanceToGo());
          float raTotal = 1.0f * abs(stepperRA.distanceToGo());
          while (stepperDEC.distanceToGo() != 0 || stepperRA.distanceToGo() != 0) {
            stepperRA.run();
            stepperDEC.run();
            // If we're not moving RA we should run the TRK stepper (same one at slow speed)
            if (raTotal == 0) { 
              stepperTRK.setSpeed(trackingspeed);
              stepperTRK.runSpeed();
            }
            if (display <= 0) {
              lcd.setCursor(0, 1);
              String disp = "";
              if (decTotal > 0)              {
                float decDist = 100.0 - 100.0 * abs(stepperDEC.distanceToGo()) / decTotal;
                disp = disp + format("DEC:%d%% ", (int)floor(decDist));
              }
              if (raTotal > 0) {
                float raDist = 100.0 - 100.0 * abs(stepperRA.distanceToGo()) / raTotal;
                disp = disp + format("RA:%d%%  ", (int)floor(raDist));
              }
              lcdMenu.printMenu(disp);
              display = displaySkip;
            }
            display--;
          }
        }

        if (lcdMenu.getActive() == HA_Menu) {
          HATime = DayTime(0, 0, 0);

          hourHA = 0;
          minHA = 0;
        }

        if (lcdMenu.getActive() == Polaris_Menu) {
          hPolarisPosition = 2 - RAAdjustedTime.getHours();
          mPolarisPosition = 57 - RAAdjustedTime.getMinutes();
          if (mPolarisPosition < 0) {
            hPolarisPosition -= 1;
            mPolarisPosition += 60;
          }
          if (hPolarisPosition < -6) {
            hPolarisPosition += 24;
            stepperDEC.moveTo(213.4 * 2);
          }
          else stepperDEC.moveTo(-213.4 * 2);

          if (hPolarisPosition > 6) hPolarisPosition -= 12;
          int hPolarisMoveTo = (hPolarisPosition * stepsPerHour + mPolarisPosition * (stepsPerHour / 60)) / 2 ;
          stepperRA.moveTo(-hPolarisMoveTo);   //             (float(4096) / float(60)));    alt

          float decTotal = 1.0f * abs(stepperDEC.distanceToGo());
          float raTotal = 1.0f * abs(stepperRA.distanceToGo());

          while (stepperRA.distanceToGo() != 0 || stepperDEC.distanceToGo() != 0) {
            stepperRA.run();
            stepperDEC.run();
            if (display == 0) {
              lcd.setCursor(0, 1);
              String disp = "";
              if (decTotal > 0)              {
                float decDist = 100.0 - 100.0 * abs(stepperDEC.distanceToGo()) / decTotal;
                disp = disp + format("DEC:%d%% ", (int)floor(decDist));
              }
              if (raTotal > 0) {
                float raDist = 100.0 - 100.0 * abs(stepperRA.distanceToGo()) / raTotal;
                disp = disp + format("RA:%d%%  ", (int)floor(raDist));
              }
              lcdMenu.printMenu(disp);
              display = displaySkip;
            }
            display--;
          }
        }

        if (lcdMenu.getActive() == Heat_Menu) {
          if (heatselect == 0) {
            RAheat = 1 - RAheat ;
          }
          if (heatselect == 1) {
            DECheat = 1 - DECheat ;
          }
          break;
        }

        if (lcdMenu.getActive() == Calibration_Menu) {

          EEPROM.update(0, inputcal);
          break;
        }

        if (lcdMenu.getActive() == Control_Menu) {
          stepperRA.move(200);
          waitForButtonRelease = false;
        }

        break;
      }


    case btnLEFT: {
        if (lcdMenu.getActive() == RA_Menu) {
          RAselect = adjustWrap(RAselect, 1, 0, 2);
        }
        if (lcdMenu.getActive() == DEC_Menu) {
          DECselect = adjustWrap(DECselect, 1, 0, 2);
        }
        if (lcdMenu.getActive() == HA_Menu) {
          HAselect = adjustWrap(HAselect, 1, 0, 1);
        }
        if (lcdMenu.getActive() == Heat_Menu) {
          heatselect = 1 - heatselect ;
        }
        waitForButtonRelease = true;

        if (lcdMenu.getActive() == Control_Menu) {
          stepperRA.move(-200);
          waitForButtonRelease = false;
        }

        break;
      }

    case btnRIGHT: {

        // Leaving Control Menu, so set stepper motor positions to zero.
        if (lcdMenu.getActive() == Control_Menu)
        {
          stepperRA.setCurrentPosition(0);
          stepperDEC.setCurrentPosition(0);
        }

        // Leaving HA Menu, so save settings to EEPROM.
        if (lcdMenu.getActive() == HA_Menu) {
          EEPROM.update(1, HATime.getHours());
          EEPROM.update(2, HATime.getMinutes());
        }

        lcdMenu.setNextActive();
        waitForButtonRelease = true;
        break;
      }
  }

  if (waitForButtonRelease) {
    while (read_LCD_buttons() != btnNONE) {
      delay(20);
    }
  }
