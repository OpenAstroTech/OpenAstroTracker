void serialEvent() {

  while (stepperGUIDE.distanceToGo() != 0) tracking = 0;

  while (Serial.available() > 0) {


    String inCmd = Serial.readStringUntil('#');
    /*lcd.setCursor(0, 0);
      lcd.print(inCmd);*/
    logString += inCmd + "\n\r";



    if (inCmd.indexOf('S') > 0) {
      int rh = inCmd.indexOf('\a');
      String a = inCmd.substring(0, rh);
      int RaH = a.toInt();

      int rm = inCmd.indexOf('\b');
      String b = inCmd.substring(rh + 1, rm);
      int RaM = b.toInt();

      int rs = inCmd.indexOf('\f');
      String c = inCmd.substring(rm + 1, rs);
      int RaS = c.toInt();

      RATime.set(RaH,RaH,RaS);

      //lcd.clear();
      //lcd.print(moveRA);
      //lcd.print(minRA);
      //lcd.print(secRA);
    }



    if (inCmd.indexOf("R") > 0) {
      int r = inCmd.indexOf('\a');
      String a = inCmd.substring(0, r);
      int amount = a.toInt();
      tracking = 0;
      stepperRA.move(amount);
      stepperRA.runToPosition();
      tracking = 1;
    }

    if (inCmd.indexOf("L") > 0) {
      tracking = 0;
      int l = inCmd.indexOf('\a');
      String a = inCmd.substring(0, l);
      int amount = a.toInt();

      stepperRA.move(-amount);
      stepperRA.runToPosition();
      tracking = 1;
    }
    if (inCmd.indexOf("UP") > 0) {
      int u = inCmd.indexOf('\a');
      String a = inCmd.substring(0, u);
      int amount = a.toInt();
      stepperDEC.setMaxSpeed(250);
      stepperDEC.setAcceleration(600);
      stepperDEC.move(-amount);
      stepperDEC.runToPosition();
    }
    if (inCmd.indexOf("DOWN") > 0) {
      int d = inCmd.indexOf('\a');
      String a = inCmd.substring(0, d);
      int amount = a.toInt();
      stepperDEC.setMaxSpeed(250);
      stepperDEC.setAcceleration(600);
      stepperDEC.move(amount);
      stepperDEC.runToPosition();
    }
    if (inCmd == "START") {
      pcControl = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PC CONTROL");
      lcd.setCursor(0, 1);
      //lcd.print(amount);
    }

    if (inCmd == "STOP") {
      pcControl = false;
      lcd.clear();
    }

    if (inCmd == "I")
    {
      if (isPulseGuiding)
      {
        Serial.println("TRUE#");
      }
      else
      {
        Serial.println("FALSE#");
      }
    }



    if (inCmd == "E") //RA + EAST
    {
      inCmd = "";

      while (inCmd.toInt() == 0) {
        tracking = 0;
        direction_new = 1;

        inCmd = Serial.readStringUntil('#');

        stepperTRK.setSpeed(50);
        stepperTRK.move(2);
        stepperTRK.runToPosition();
        tracking = 1;
        logString += inCmd + "\n\r";
      }
    }

    if (inCmd == "W") //RA - WEST
    {
      inCmd = "";

      while (inCmd.toInt() == 0) {
        tracking = 0;
        direction_new = 0;


        inCmd = Serial.readStringUntil('#');

        stepperTRK.setAcceleration(2000);
        stepperTRK.setMaxSpeed(800);
        stepperTRK.setSpeed(800);
        stepperTRK.move(-12);
        stepperTRK.runToPosition();
        stepperTRK.move(10);
        stepperTRK.runToPosition();
        tracking = 1;
        logString += inCmd + "\n\r";
      }
    }

    if (inCmd == "U") //DEC + NORTH
    {
      inCmd = "";

      while (inCmd.toInt() == 0)
        inCmd = Serial.readStringUntil('#');

      logString += inCmd + "\n\r";

      stepperDEC.move(-1);
      stepperDEC.runToPosition();
    }
    if (inCmd == "D") //DEC - SOUTH
    {
      inCmd = "";

      while (inCmd.toInt() == 0)
        inCmd = Serial.readStringUntil('#');

      logString += inCmd + "\n\r";

      stepperDEC.move(1);
      stepperDEC.runToPosition();
    }
    if (inCmd == "N") {

      isPulseGuiding = false;
    }

    if (inCmd == "L")  {
      Serial.println(logString);
      logString = "";
    }


    // Stellarium stuff--------------------------------------------------

    if (inCmd == ":GR") {
      sprintf(current_RA, "%02d:%02d:%02d", RAAdjustedTime.getHours(), RAAdjustedTime.getMinutes(), RAAdjustedTime.getSeconds());
      Serial.print(current_RA);
      Serial.print("#");
      //inCmd = "";
    }

    if (inCmd == ":GD") {
      sprintf(current_DEC, "%c%02d*%02d:%02d", '+', int(printdegDEC), int(minDEC), int(secDEC));
      Serial.print(current_DEC);
      //Serial.print("+80*00#");
      Serial.print("#");
      //inCmd = "";
    }
    if (inCmd.indexOf("Sr") > 0) {
      String x = inCmd.substring(3);
      int RaH = x.toInt();
      String y = inCmd.substring(7);
      int RaM = y.toInt();
      String z = inCmd.substring(10);
      int RaS = z.toInt();

      int slew_RAh = (RaH - RAAdjustedTime.getHours());
      int slew_RAm = (RaM - RAAdjustedTime.getMinutes());
      int slew_RAs = (RaS - RAAdjustedTime.getSeconds());

      RATime.addTime(slew_RAh,slew_RAm,slew_RAs);

      Serial.print("1");
      slew_RA = (slew_RAh * onehour + slew_RAm * (onehour / 60) + slew_RAs * (onehour / 3600)) / 2;

      stepperRA.moveTo(slew_RA);
      //inCmd = "";
      //slew();
    }
    if (inCmd.indexOf(":Sd") >= 0) {
      //int s = inCmd.indexOf ("Sd");
      String x = inCmd.substring(4);
      int DECd = x.toInt();
      String y = inCmd.substring(8);
      int DECm = y.toInt();
      String z = inCmd.substring(11);
      int DECs = z.toInt();

      int slew_DECd = (printdegDEC - DECd);
      int slew_DECm = (DECm - minDEC);
      int slew_DECs = (DECs - secDEC);

      degreeDEC += slew_DECd;
      minDEC += slew_DECm;
      secDEC += slew_DECs;
      Serial.print("1");
      slew_DEC = (slew_DECd * float(DECsteps) + slew_DECm * (float(DECsteps) / float(60)) + slew_DECs * (float(DECsteps) / float(3600))) / 2;

      stepperDEC.moveTo(slew_DEC);
      //inCmd = "";
      //slew();
    }
    if (inCmd == ":MS") {
      Serial.print(0);
      inCmd = "";
      while (stepperRA.distanceToGo() != 0  && stepperDEC.distanceToGo() == 0) {
        stepperRA.run();
      }
      while (stepperDEC.distanceToGo() != 0 && stepperRA.distanceToGo() == 0) {
        stepperDEC.run();

      }
      while (stepperDEC.distanceToGo() != 0 || stepperRA.distanceToGo() != 0) {
        stepperRA.run();
        stepperDEC.run();
      }
    }
    loop();

  }
}
