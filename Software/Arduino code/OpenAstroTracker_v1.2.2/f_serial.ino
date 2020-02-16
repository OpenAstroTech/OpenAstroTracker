void serialEvent() {

  while (stepperGUIDE.distanceToGo() != 0) tracking = 0;

  while (Serial.available() > 0) {
    String inCmd;
    inCmd = Serial.readStringUntil('#');


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

      hourRA = RaH;
      minRA = RaM;
      secRA = RaS;

      //lcd.clear();
      //lcd.print(moveRA);
      //lcd.print(minRA);
      //lcd.print(secRA);


    }

    if (inCmd.indexOf('R') > 0) {
      int r = inCmd.indexOf('\a');
      String a = inCmd.substring(0, r);
      int amount = a.toInt();
      tracking = 0;
      stepperRA.move(amount);
      stepperRA.runToPosition();
      tracking = 1;
    }

    if (inCmd.indexOf('L') > 0) {
      tracking = 0;
      int l = inCmd.indexOf('\a');
      String a = inCmd.substring(0, l);
      int amount = a.toInt();

      stepperRA.move(-amount);
      stepperRA.runToPosition();
      tracking = 1;
    }
    if (inCmd.indexOf('UP') > 0) {
      int u = inCmd.indexOf('\a');
      String a = inCmd.substring(0, u);
      int amount = a.toInt();
      stepperDEC.setMaxSpeed(250);
      stepperDEC.setAcceleration(600);
      stepperDEC.move(-amount);
      stepperDEC.runToPosition();
    }
    if (inCmd.indexOf('DOWN') > 0) {
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

    /*if (inCmd = "G") {
      lcd.print("get");
      Serial.write("get");
    }*/

    if (inCmd == "E") //RA + EAST
    {
      inCmd = "";

      while (inCmd.toInt() == 0) {
        tracking = 0;
        direction_new = 1;

        /*if (direction_new != direction_old) {
          stepperTRK.setAcceleration(1000);
          stepperTRK.setMaxSpeed(500);
          stepperTRK.setSpeed(150);
          stepperTRK.move(40);
          stepperTRK.runToPosition();
          direction_old = direction_new;
          tracking = 1;
          break;
          }*/

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

        /*if (direction_new != direction_old) {
          stepperTRK.setAcceleration(1000);
          stepperTRK.setMaxSpeed(500);
          stepperTRK.setSpeed(150);
          stepperTRK.move(-40);
          stepperTRK.runToPosition();
          direction_old = direction_new;
          tracking = 1;
          break;
          }*/

        inCmd = Serial.readStringUntil('#');

        stepperTRK.setAcceleration(1000);
        stepperTRK.setMaxSpeed(500);
        stepperTRK.setSpeed(200);
        stepperTRK.move(-34);
        stepperTRK.runToPosition();
        stepperTRK.move(32);
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

      stepperDEC.move(-2);
      stepperDEC.runToPosition();
    }
    if (inCmd == "D") //DEC - SOUTH
    {
      inCmd = "";

      while (inCmd.toInt() == 0)
        inCmd = Serial.readStringUntil('#');

      logString += inCmd + "\n\r";

      stepperDEC.move(2);
      stepperDEC.runToPosition();
    }
    if (inCmd == "N")
    {
      /*OCR1A = trackRate;
        TIMSK1 |= (1 << OCIE1A);
        TCNT1 = 0; //Reset counter*/
      isPulseGuiding = false;

    }

    if (inCmd == "L")
    {
      Serial.println(logString);
      logString = "";
    }


  }
}
