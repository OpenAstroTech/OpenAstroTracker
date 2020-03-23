void loop() {
  bool waitForButtonRelease = false;
  //if (adc_key_in < 1000) delay(150);
  lcd_key = read_LCD_buttons();
  //speedcalibration += inputcal / 1000;
  trackingspeed = ((((335.1417 / 288.0) * RevSteps) / 3590)) - 1 + float(speedcalibration);

  onehour = (float(RAsteps) / 288.0) * RevSteps;
  moveRA = (RATime.getHours() * onehour + RATime.getMinutes()* (onehour / 60.0f) + RATime.getSeconds()* (onehour / 3600.0f)) / 2;
  moveDEC = (degreeDEC * float(DECsteps) + minDEC * (float(DECsteps) / 60.0f) + secDEC * (float(DECsteps) / 3600.0f));

  if (moveRA > (6 * onehour / 2)) {         //turn both RA and DEC axis around if target is below horizontal parallel
    moveRA -= long(12 * onehour / 2);
    moveDEC = -moveDEC;
  }
  if (moveRA < (-6 * onehour / 2)) {
    moveRA += long(12 * onehour / 2);
    moveDEC = -moveDEC;
  }

  //if ((moveRA != 0) || (moveDEC != 0)) {
  if (controlDisplay == 1) {
    //Serial.println("DEC: " + String(stepperDEC.currentPosition()) + "  RA: " + String(stepperRA.currentPosition()));
  }
  //}

  if (lcdMenu.getActive() != Control_Menu) {
    stepperRA.moveTo(-moveRA);
    stepperDEC.moveTo(moveDEC);
  }

  switch (lcd_key) {
    case btnUP: {
        if (lcdMenu.getActive() == RA_Menu) {
          if (RAselect == 0) RATime.addHours(1);
          if (RAselect == 1) RATime.addMinutes(1);
          if (RAselect == 2) RATime.addSeconds(1);
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
          if (RAselect == 1) RATime.addMinutes( -1);
          if (RAselect == 2) RATime.addSeconds( -1);
        }
        if (lcdMenu.getActive() == DEC_Menu) {
          if (DECselect == 0) degreeDEC = north ? adjustClamp(degreeDEC, -1, -180, 0) : adjustClamp(degreeDEC, -1, 0, 180);
          if (DECselect == 1) minDEC = adjustMinute(minDEC, -1);
          if (DECselect == 2) secDEC = adjustSecond(secDEC, -1);
        }
        if (lcdMenu.getActive() == HA_Menu) {
          if (HAselect == 0) HATime.addHours(-1);
          if (HAselect == 1) HATime.addMinutes(-1);
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

        if (lcdMenu.getActive() < HA_Menu) {
          while (stepperRA.distanceToGo() != 0  && stepperDEC.distanceToGo() == 0) {
            stepperRA.run();
            if (display == 0) {
              lcd.setCursor(0, 1);
              lcd.print("RA: " + String(stepperRA.distanceToGo()) + "  ");
              display = displaySkip;
            }
            display--;
          }
          while (stepperDEC.distanceToGo() != 0 && stepperRA.distanceToGo() == 0) {
            stepperDEC.run();
            stepperTRK.setSpeed(trackingspeed);
            stepperTRK.runSpeed();
            if (display == 0) {
              lcd.setCursor(0, 1);
              lcd.print("DEC: " + String(stepperDEC.distanceToGo()) + "  ");
              display = displaySkip;
            }
            display--;
          }
          while (stepperDEC.distanceToGo() != 0 || stepperRA.distanceToGo() != 0) {
            stepperRA.run();
            stepperDEC.run();
            if (display == 0) {
              lcd.setCursor(0, 1);
              lcd.print("DEC:" + String(abs(stepperDEC.distanceToGo()) / 10) + "  RA:" + String(abs(stepperRA.distanceToGo()) / 10) + "   ");
              display = displaySkip;
            }
            display--;
          }
        }

        if (lcdMenu.getActive() == HA_Menu) {
          HATime = DayTime(0, 0, 0);
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
          int hPolarisMoveTo = (hPolarisPosition * onehour + mPolarisPosition * (onehour / 60)) / 2 ;
          stepperRA.moveTo(-hPolarisMoveTo);   //             (float(4096) / float(60)));    alt

          while (stepperRA.distanceToGo() || stepperDEC.distanceToGo() != 0) {
            stepperRA.run();
            stepperDEC.run();
            if (display == 0) {
              lcd.setCursor(0, 1);
              lcd.print("DEC:" + String(abs(stepperDEC.distanceToGo()) / 10) + "  RA:" + String(abs(stepperRA.distanceToGo()) / 10) + "   ");
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
