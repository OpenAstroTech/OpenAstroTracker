if (!pcControl) {
  lcd.setCursor(0, 0);

  lcdMenu.updateDisplay();

  lcd.setCursor(0, 1);

  //RA Menu printing; =menu0 -------------------------------

  //Serial.println("TO GO DEC:" + String(stepperDEC.distanceToGo()) + "  RA:" + String(stepperRA.distanceToGo()));

  if (lcdMenu.getActive() == RA_Menu) {

    //Serial.println("HA:" + HATime.ToString() + "  HA(corr):" + HACorrection.ToString() + "  RA:" + RATime.ToString() + "  RA(corr):" + RAAdjustedTime.ToString());
//    if (hourRAprint < 0) hourRAprint += 24;
//    if (minRAprint < 0) minRAprint += 60;
    if (RAselect == 0) {
      lcdMenu.printMenuArg(">%dh %dm %ds", RAAdjustedTime.getHours(), RAAdjustedTime.getMinutes(), RAAdjustedTime.getSeconds());

      //lcd.print(int(diamCorrection));   //for debugging
      //lcd.print("");
      //lcd.print(trackingspeed);
      //lcd.print("   ");
    }
    if (RAselect == 1) {
      lcdMenu.printMenuArg(" %dh>%dm %ds", RAAdjustedTime.getHours(), RAAdjustedTime.getMinutes(), RAAdjustedTime.getSeconds());
    }
    if (RAselect == 2) {
      lcdMenu.printMenuArg(" %dh %dm>%ds", RAAdjustedTime.getHours(), RAAdjustedTime.getMinutes(), RAAdjustedTime.getSeconds());
    }
  }

  //DEC menu printing =menu1  ------------------------------
  if (lcdMenu.getActive() == DEC_Menu) {
    if (DECselect == 0) {
      lcdMenu.printMenuArg(">%d%c %d%c %d%c", printdegDEC, 0, minDEC, 1, secDEC, 2);
    }
    if (DECselect == 1) {
      lcdMenu.printMenuArg(" %d%c>%d%c %d%c", printdegDEC, 0, minDEC, 1, secDEC, 2);
    }
    if (DECselect == 2) {
      lcdMenu.printMenuArg(" %d%c %d%c>%d%c", printdegDEC, 0, minDEC, 1, secDEC, 2);
    }
  }

  //Homing menu  ------------------------
  /*if (menu == Homing_Menu) {
    //lcd.print("Select for home");   Doesnt work yet
    lcd.print("Work in Progress");
    lcd.print("         ");
    }*/

  //tracking menu  ----------------------
  /*if (menu == 3) {
    lcd.print("Tracking ON OFF");
    lcd.print("         ");
    }*/

  //heating menu   -----------------------
  if (lcdMenu.getActive() == Heat_Menu) {
    if (heatselect == 0 && RAheat == 0 && DECheat == 0) {
      lcdMenu.printMenu("RA>OFF< DEC: OFF ");
    }
    if (heatselect == 0 && RAheat == 1 && DECheat == 0) {
      lcdMenu.printMenu("RA>ON < DEC: OFF ");
    }
    if (heatselect == 0 && RAheat == 0 && DECheat == 1) {
      lcdMenu.printMenu("RA>OFF< DEC: ON  ");
    }
    if (heatselect == 0 && RAheat == 1 && DECheat == 1) {
      lcdMenu.printMenu("RA>ON < DEC: ON  ");
    }
    if (heatselect == 1 && RAheat == 0 && DECheat == 0) {
      lcdMenu.printMenu("RA:OFF  DEC>OFF< ");
    }
    if (heatselect == 1 && RAheat == 1 && DECheat == 0) {
      lcdMenu.printMenu("RA:ON   DEC>OFF< ");
    }
    if (heatselect == 1 && RAheat == 0 && DECheat == 1) {
      lcdMenu.printMenu("RA:OFF  DEC>ON< ");
    }
    if (heatselect == 1 && RAheat == 1 && DECheat == 1) {
      lcdMenu.printMenu("RA:ON   DEC>ON< ");
    }
  }

  //HA menu printing =menu2  -----------------------------
  if (lcdMenu.getActive() == HA_Menu) {
    if (HAselect == 0) {
      lcdMenu.printMenuArg(">%dh %dm", HATime.getHours(), HATime.getMinutes());
    }
    if (HAselect == 1) {
      lcdMenu.printMenuArg(" %dh>%dm", HATime.getHours(), HATime.getMinutes());
    }
  }

  //Calibration menu   -----------------------------
  speedcalibration = speed + inputcal / 10000;
  if (lcdMenu.getActive() == Calibration_Menu) {
    lcd.print("Speed:");
    lcd.print(speedcalibration, 4);
    lcd.print("        ");
  }

  //Polaris menu + calculations -----------------------
  hPolarisPosition = 2 - RAAdjustedTime.getHours();
  mPolarisPosition = 57 - RAAdjustedTime.getMinutes();
  if (mPolarisPosition < 0) {
    hPolarisPosition -= 1;
    mPolarisPosition += 60;
  }
  if (hPolarisPosition < -6) hPolarisPosition += 24;
  if (hPolarisPosition > 6) hPolarisPosition -= 12;
  if (lcdMenu.getActive() == Polaris_Menu) {
    /*lcd.print(hPolarisPosition);
      lcd.print("  ");
      lcd.print(mPolarisPosition);*/

    lcd.print("Sel for Polaris");
    //lcd.print(inCmd);
    lcd.print("         ");
    /*lcd.print(int(hourRAprint));    //debugging
      lcd.print(" ");
      lcd.print(hPolarisPosition);
      lcd.print("  ");
      lcd.print(int(minRAprint));
      lcd.print(" ");
      lcd.print(mPolarisPosition);
      lcd.print("  ");*/
  }


  //Ctrl menu printing   -----------------------------
  if (lcdMenu.getActive() == Control_Menu) {
    //Serial.println(String(hourRA)+":"+String(minRA)+" RA:"+String(stepperRA.distanceToGo() )+ "  "+String(degreeDEC)+":"+String(minDEC)+"\" DEC:"+String(stepperDEC.distanceToGo()));
    if ((stepperRA.distanceToGo() != 0)  || (stepperDEC.distanceToGo() != 0)) {
      if (stepperRA.distanceToGo() != 0  ) {
        stepperRA.run();
      }
      if (stepperDEC.distanceToGo() != 0  ) {
        stepperDEC.run();
      }
      if (controlDisplay <= 0) {
        lcd.setCursor(0, 1);
        lcdMenu.printMenuArg("DEC:%d RA:%d", stepperDEC.distanceToGo(), stepperRA.distanceToGo());
        controlDisplay = 30;
      }
      controlDisplay --;
    }
    else {
      lcdMenu.printMenuArg("DEC:%d RA:%d", stepperDEC.currentPosition(), stepperRA.currentPosition());
      //lcdMenu.printMenuArg("Use buttons");
      //stepperRA.setCurrentPosition(0);
      //stepperDEC.setCurrentPosition(0);
    }
  }
}
BTin();
}
