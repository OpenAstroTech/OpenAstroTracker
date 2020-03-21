if (!pcControl) {
  lcd.setCursor(0, 0);

  if (north == true) {
    if (menu == 0) printMenu("     >RAs< DEC H");
    if (menu == 1) printMenu(" RAs >DEC< HA PO");
    if (menu == 2) printMenu(" DEC >HA < POL H");
    if (menu == 3) printMenu("  HA >POL< HEAT ");
    if (menu == 4) printMenu(" POL >HEAT< CAL ");
    if (menu == 5) printMenu("HEAT >CAL<      ");
  }
  else {
    if (menu == 0) printMenu("     >RAs< DEC H");
    if (menu == 1) printMenu(" RAs >DEC< HA HE");
    if (menu == 2) printMenu(" DEC >HA < HEAT ");

    if (menu == 4) printMenu(" HA  >HEAT< CAL ");
    if (menu == 5) printMenu("HEAT >CAL<      ");
  }



  lcd.setCursor(0, 1);


  //RA Menu printing; =menu0 -------------------------------

  if (menu == 0) {
    if (hourRAprint < 0) hourRAprint += 24;
    if (minRAprint < 0) minRAprint += 60;
    if (RAselect == 0) {
      lcd.write(rightArrow);
      lcd.print(int(hourRAprint));
      lcd.print("h ");
      lcd.print(int(minRAprint));
      lcd.print("m ");
      lcd.print(int(secRAprint));
      lcd.print("s");
      lcd.print("                    ");

      //lcd.print(int(diamCorrection));   //for debugging
      //lcd.print("");
      //lcd.print(trackingspeed);
      //lcd.print("   ");
    }
    if (RAselect == 1) {
      lcd.print(" ");
      lcd.print(int(hourRAprint));
      lcd.print("h");
      lcd.write(rightArrow);
      lcd.print(int(minRAprint));
      lcd.print("m ");
      lcd.print(int(secRAprint));
      lcd.print("s");
      lcd.print("         ");

    }
    if (RAselect == 2) {
      lcd.print(" ");
      lcd.print(int(hourRAprint));
      lcd.print("h");
      lcd.print(" ");
      lcd.print(int(minRAprint));
      lcd.print("m");
      lcd.write(rightArrow);
      lcd.print(int(secRAprint));
      lcd.print("s ");
      lcd.print("         ");

    }
  }
  //DEC menu printing =menu1  ------------------------------
  if (menu == 1) {
    if (DECselect == 0) {
      lcd.write(rightArrow);
      lcd.print(printdegDEC);
      lcd.write(byte(0));
      lcd.print(" ");
      lcd.print(minDEC);
      lcd.write(byte(1));
      lcd.print(" ");
      lcd.print(secDEC);
      lcd.write(byte(2));
      /*lcd.print(DECd);
        lcd.print(DECm);
        lcd.print(DECs);*/
      lcd.print("         ");
      //lcd.print(int(moveDEC));  //debugging
      //lcd.print("   ");
    }
    if (DECselect == 1) {
      lcd.print(" ");
      lcd.print(printdegDEC);
      lcd.write(byte(0));
      lcd.write(rightArrow);
      lcd.print(minDEC);
      lcd.write(byte(1));
      lcd.print(" ");
      lcd.print(secDEC);
      lcd.write(byte(2));
      lcd.print("       ");
    }
    if (DECselect == 2) {
      lcd.print(" ");
      lcd.print(printdegDEC);
      lcd.write(byte(0));
      lcd.print(" ");
      lcd.print(minDEC);
      lcd.write(byte(1));
      lcd.write(rightArrow);
      lcd.print(secDEC);
      lcd.write(byte(2));
      lcd.print("       ");
    }
  }

  //Homing menu  ------------------------
  /*if (menu == 2) {
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
  if (menu == 4) {
    if (heatselect == 0 && RAheat == 0 && DECheat == 0) {
      printMenu("RA>OFF< DEC: OFF ");
    }
    if (heatselect == 0 && RAheat == 1 && DECheat == 0) {
      printMenu("RA>ON < DEC: OFF ");
    }
    if (heatselect == 0 && RAheat == 0 && DECheat == 1) {
      printMenu("RA>OFF< DEC: ON  ");
    }
    if (heatselect == 0 && RAheat == 1 && DECheat == 1) {
      printMenu("RA>ON < DEC: ON  ");
    }
    if (heatselect == 1 && RAheat == 0 && DECheat == 0) {
      printMenu("RA:OFF  DEC>OFF< ");
    }
    if (heatselect == 1 && RAheat == 1 && DECheat == 0) {
      printMenu("RA:ON   DEC>OFF< ");
    }
    if (heatselect == 1 && RAheat == 0 && DECheat == 1) {
      printMenu("RA:OFF  DEC>ON< ");
    }
    if (heatselect == 1 && RAheat == 1 && DECheat == 1) {
      printMenu("RA:ON   DEC>ON< ");
    }
  }
  
  //HA menu printing =menu2  -----------------------------
  if (menu == 2) {
    if (HAselect == 0) {
      //lcd.print("Input HA:");
      lcd.write(rightArrow);
      lcd.print(hourHAzeit);
      lcd.print("h");
      lcd.print(" ");
      lcd.print(minHAzeit);
      lcd.print("m");
      //lcd.print(trackingspeed, 5);
      lcd.print("         ");

    }
    if (HAselect == 1) {
      //lcd.print("Input HA ");
      lcd.print(" ");
      lcd.print(hourHAzeit);
      lcd.print("h");
      lcd.write(rightArrow);
      lcd.print(minHAzeit);
      lcd.print("m");
      lcd.print("         ");
    }
  }

  //Calibration menu   -----------------------------
  speedcalibration = speed + inputcal / 10000;
  if (menu == 5) {
    lcd.print("Speed:");
    lcd.print(speedcalibration, 4);
    lcd.print("        ");
  }

  //Polaris menu + calculations -----------------------
  hPolarisPosition = 2 - hourRAprint;
  mPolarisPosition = 57 - minRAprint;
  if (mPolarisPosition < 0) {
    hPolarisPosition -= 1;
    mPolarisPosition += 60;
  }
  if (hPolarisPosition < -6) hPolarisPosition += 24;
  if (hPolarisPosition > 6) hPolarisPosition -= 12;
  if (menu == 3 && north == true) {
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



}
BTin();
}
