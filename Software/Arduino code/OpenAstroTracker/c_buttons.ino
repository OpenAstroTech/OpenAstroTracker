void loop() {

  if (adc_key_in < 1000) delay(150);
  lcd_key = read_LCD_buttons();
  //speedcalibration += inputcal / 1000;
  float trackingspeed = ((((335.1417 / 24) / 12) * RevSteps) / 3590) - 1 + float(speedcalibration);

  float onehour = float(float(RAsteps / 24) / 12) * RevSteps;
  moveRA = (hourRA * onehour + minRA * (onehour / float(60)) + secRA * (onehour / float(3600))) / 2;
  moveDEC = (degreeDEC * float(164) + minDEC * (float(164) / float(60)) + secDEC * (float(164) / float(3600))) / 2;

  if (moveRA > (6 * onehour / 2)) {         //turn both RA and DEC axis around if target is below horizontal parallel
    moveRA -= long(12 * onehour / 2);
    moveDEC = -moveDEC;
  }
  if (moveRA < (-6 * onehour / 2)) {
    moveRA += long(12 * onehour / 2);
    moveDEC = -moveDEC;
  }

  stepperRA.moveTo(-moveRA);
  stepperDEC.moveTo(moveDEC);

  switch (lcd_key) {
    case btnUP: {
        while (menu == 0) {
          if (RAselect == 0) hourRA += 1;
          if (RAselect == 1) minRA += 1;
          if (RAselect == 2) secRA += 1;
          break;
        }
        while (menu == 1) {
          if (DECselect == 0) degreeDEC += 1;
          if (DECselect == 1) minDEC += 1;
          if (DECselect == 2) secDEC += 1;
          break;
        }
        while (menu == 2) {
          if (HAselect == 0) hourHA += 1;
          if (HAselect == 1) minHA += 1;
          break;
        }
        while (menu == 5) {
          inputcal += 1;  //0.0001;

          break;
        }
        break;
      }

    case btnDOWN: {
        while (menu == 0) {
          if (RAselect == 0) hourRA -= 1;
          if (RAselect == 1) minRA -= 1;
          if (RAselect == 2) secRA -= 1;
          break;
        }
        while (menu == 1) {
          if (DECselect == 0) degreeDEC -= 1;
          if (DECselect == 1) minDEC -= 1;
          if (DECselect == 2) secDEC -= 1;
          break;
        }
        while (menu == 5) {
          inputcal -= 1 ; //0.0001;

          break;
        }
        //while (menu == 2) {
        //  break;

        /*while (menu == 4) {              // only counting up recommended, breaks code otherwise
          if (HAselect == 0) hourHA -= 1;
          if (HAselect == 1) minHA -= 1;
          break;
          }*/
        break;
      }

    case btnSELECT: {
        /*stepperRA.moveTo(-moveRA);
          stepperDEC.moveTo(moveDEC);*/
        if (menu < 2) {
          while (stepperRA.distanceToGo() != 0  && stepperDEC.distanceToGo() == 0) {
            stepperRA.run();
          }
          while (stepperDEC.distanceToGo() != 0 && stepperRA.distanceToGo() == 0) {
            stepperDEC.run();
            stepperTRK.setSpeed(trackingspeed);
            stepperTRK.runSpeed();
          }
          while (stepperDEC.distanceToGo() != 0 || stepperRA.distanceToGo() != 0) {
            stepperRA.run();
            stepperDEC.run();
          }
        }

        if (menu == 3) {
          hPolarisPosition = 2 - hourRAprint;
          mPolarisPosition = 57 - minRAprint;
          if (mPolarisPosition < 0) {
            hPolarisPosition -= 1;
            mPolarisPosition += 60;
          }
          if (hPolarisPosition < -6) {
            hPolarisPosition += 24;
            stepperDEC.moveTo(213.4);
          }
          else stepperDEC.moveTo(-213.4);

          if (hPolarisPosition > 6) hPolarisPosition -= 12;
          int hPolarisMoveTo = (hPolarisPosition * onehour + mPolarisPosition * (onehour / 60)) / 2 ;
          stepperRA.moveTo(-hPolarisMoveTo);   //             (float(4096) / float(60)));    alt

          while (stepperRA.distanceToGo() || stepperDEC.distanceToGo() != 0) {
            stepperRA.run();
            stepperDEC.run();
          }
        }

        if (menu == 4) {
          if (heatselect == 0) {
            RAheat += 1;
            if (RAheat > 1) RAheat = 0;
          }
          if (heatselect == 1) {
            DECheat += 1;
            if (DECheat > 1) DECheat = 0;
          }
          break;
        }
        if (menu == 5) {

          EEPROM.update(0, inputcal);
          break;
        }
        break;
      }


    case btnLEFT: {
        if (menu == 0) {
          RAselect += 1;
          while (RAselect > 2) RAselect = 0;
          break;
        }
        if (menu == 1) {
          DECselect += 1;
          while (DECselect > 2) DECselect = 0;
        }
        if (menu == 2) {
          HAselect += 1;
          while (HAselect > 1) HAselect = 0;
          break;
        }
        if (menu == 4) {
          heatselect += 1;
          while (heatselect > 1) heatselect = 0;
        }
        break;
      }

    case btnRIGHT: {
        menu += 1;
        while (menu > 5) menu = 0;
        if (!north && menu == 3) menu = 4;
        break;
        

      }
  }
