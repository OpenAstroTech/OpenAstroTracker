
void startSlewing() {
  serialIsSlewing = true;
  doCalculations();
  handleDECandRACalculations();
  startMoveSteppersToTargetAsync();
}


/////////////////////////////
// INIT
/////////////////////////////
void handleMeadeInit(String inCmd) {
  inSerialControl = true;
  lcdMenu.clear();
  lcdMenu.setCursor(0, 0);
  lcdMenu.printMenu("Remote control");
  lcdMenu.setCursor(0, 1);
  lcdMenu.printMenu(">SELECT to quit");
}

/////////////////////////////
// GET INFO
/////////////////////////////
void handleMeadeGetInfo(String inCmd) {
  char cmdOne = inCmd[0];
  char cmdTwo = (inCmd.length() > 1) ? inCmd[1]  : '\0';

  switch (cmdOne) {
    case 'V' : {
        if (cmdTwo == 'N') {
          Serial.print(version);
          Serial.print("#");
        }
        else if (cmdTwo == 'P') {
          Serial.print("OpenAstroTracker#");
        }
      }
      break;
    case 'r': {
        sprintf(current_RA, "%02d:%02d:%02d#", RADisplayTime.getHours(), RADisplayTime.getMinutes(), RADisplayTime.getSeconds());
        Serial.print(current_RA);
      }
      break;
    case 'd': {
        sprintf(current_DEC, "%c%02d*%02d'%02d#", printdegDEC > 0 ? '+' : '-', int(fabs(printdegDEC)), int(minDEC), int(secDEC));
        Serial.print(current_DEC);
      }
      break;
  }
}

/////////////////////////////
// SET INFO
/////////////////////////////
void handleMeadeSetInfo(String inCmd) {
  if (inCmd.length() < 8) {
    Serial.print("0");
    return;
  }
  if (inCmd[0] == 'd') {
    // Set DEC
    int sgn = inCmd[1] == '+' ? 1 : -1;
    if ((inCmd[4] == '*') && (inCmd[7] == ':') )
    {
      int deg = inCmd.substring(2, 4).toInt();
      degreeDEC = sgn * deg + (north ? -90 : 90);
      minDEC = inCmd.substring(5, 7).toInt();
      secDEC =  inCmd.substring(8, 10).toInt();
      doCalculations();
      handleDECandRACalculations();
      Serial.print(isUnreachable ? "0" : "1");
    } else  {
      // Did not understand the coordinate
      Serial.print("0");
    }
  } else if (inCmd[0] == 'r') {
    // Set RA
    if ((inCmd[3] == ':') && (inCmd[6] == ':') )
    {
      int hRA = inCmd.substring(1, 3).toInt();
      int minRA = inCmd.substring(4, 6).toInt();
      int secRA = inCmd.substring(7, 9).toInt();

      RATime.set(hRA, minRA, secRA);
      RATime.subtractTime(HACorrection);
      doCalculations();
      handleDECandRACalculations();
      Serial.print(isUnreachable ? "0" : "1");
    } else
      // Did not understand the coordinate
      Serial.print("0");
  } else if (inCmd[0] == 'H') {
    // Set HA
    int hHA = inCmd.substring(1, 3).toInt();
    int minHA = inCmd.substring(4, 6).toInt();
    HATime.set(hHA, minHA, 0);
    HACorrection.set(HATime);
    HACorrection.addTime(-h, -m, -s);
  }
}

/////////////////////////////
// MOVEMENT
/////////////////////////////
void handleMeadeMovement(String inCmd) {
  if (inCmd[0] == 'S') {
    Serial.print("0");
    startSlewing();
  }
}

/////////////////////////////
// HOME
/////////////////////////////
void handleMeadeHome(String inCmd) {
  if (inCmd[0] == 'P') {
    stopSteppers();

    // TRK stepper is half-stepped so we divide the steps by two to get full steps, which is what RA is stepped at.
    stepperRA.moveTo(0 - stepperTRK.currentPosition() / 2);
    stepperDEC.moveTo(0);
    serialIsSlewing = true;
    startMoveSteppersToTargetAsync();
  }
}

/////////////////////////////
// QUIT
/////////////////////////////
void handleMeadeQuit(String inCmd) {

  // Hard quit extension stops motors for :Q# command, but not for :Qq#
  if ((inCmd.length() == 0) || (inCmd[0] != 'q'))  {
    stopSteppers();
  }

  serialIsSlewing = false;
  inSerialControl = false;

  lcdMenu.setCursor(0, 0);
  lcdMenu.updateDisplay();
}

////////////////////////////////////////////////
// The main loop when under serial control
void serialLoop()
{
  runTracker();
  if (serialIsSlewing) {
    if (!moveSteppersToTargetAsync()) {
      serialIsSlewing = false;
    }
  }
}

//////////////////////////////////////////////////
// Event that is triggered when the serial port receives data.
void serialEvent() {

  if (stepperGUIDE.distanceToGo() != 0) tracking = 0;

  // Implement Meade protocol
  while (Serial.available() > 0) {

    String inCmd = Serial.readStringUntil('#');
    logString += inCmd + "\n\r";

    if (inCmd[0] = ':')
    {

      char command = inCmd[1];
      inCmd = inCmd.substring(2);
      switch (command) {
        case 'S' : handleMeadeSetInfo(inCmd); break;
        case 'M' : handleMeadeMovement(inCmd); break;
        case 'G' : handleMeadeGetInfo(inCmd); break;
        case 'h' : handleMeadeHome(inCmd); break;
        case 'I' : handleMeadeInit(inCmd); break;
        case 'Q' : handleMeadeQuit(inCmd); break;
      }
    }

    /*
      int cmdSep = inCmd.indexOf(' | ');
      if (cmdSep >= 0) {
      // Our own protocol format. Currently only setting RA and DEC and manual slew is supported.
      // So available formats are:
      //     arg,arg,arg|CMD
      // OR
      //     |CMD

      String command = inCmd.substring(cmdSep + 1);
      int commandId = -1;
      for (int i = 0; i < sizeof(supportedCommands) / sizeof(supportedCommands[0]); i++)
      {
      if (supportedCommands[i].command == command) {
      commandId = supportedCommands[i].commandId;
      break;
      }
      }

      String argString = inCmd.substring(0, cmdSep);
      int args[3];
      int argIndex = 0;

      // Check for arguments
      while (argString.length() > 0 ) {
      int commaPos = argString.indexOf(', ');
      if (commaPos == -1) {
      args[argIndex++] = argString.toInt();
      argString = "";
      }
      else {
      args[argIndex++] = argString.substring(0, commaPos).toInt();
      argString = argString.substring(commaPos + 1);
      }
      }

      switch (commandId)
      {
      // SRA command - Set RA time
      case MOVE_TO_RA : {
      if (argIndex == 3)
      {
      RATime.set(args[0], args[1], args[2]);
      doCalculations();
      lcdMenu.setCursor(0, 1);
      sprintf(current_RA, "%02d:%02d:%02d", RADisplayTime.getHours(), RADisplayTime.getMinutes(), RADisplayTime.getSeconds());
      lcdMenu.printMenu("RA : " + String(current_RA));
      //ShowStatusMessage();
      handleDECandRACalculations();
      startMoveSteppersToTargetAsync() ;
      }
      }
      break;

      case MOVE_TO_DEC : {
      if (argIndex == 3) {
      degreeDEC = args[0] + (north ? -90 : 90);
      minDEC = args[1];
      secDEC = args[2];
      doCalculations();
      lcdMenu.setCursor(0, 1);
      lcdMenu.printMenuArg("DEC: %d%c %d%c %d%s", printdegDEC, 0, minDEC, 1, secDEC, "\"");
      //ShowStatusMessage();
      handleDECandRACalculations();
      startMoveSteppersToTargetAsync() ;
      }
      }
      break;

      case PC_START : {
      // START command - Go to PC Control
      pcControl = true;
      lcdMenu.clear();
      lcdMenu.setCursor(0, 0);
      lcdMenu.print("PC CONTROL");
      lcdMenu.setCursor(0, 1);
      }
      break;

      case PC_STOP: {
      // STOP command - exit PC Control
      pcControl = false;
      lcdMenu.clear();
      }
      break;

      case MOVE_RIGHT: {
      // R command - Move RA stepper right
      if (argIndex == 1) {
      //tracking = 0;
      stepperRA.move(args[0]);
      //stepperRA.runToPosition();
      //tracking = 1;
      }
      }
      break;

      case MOVE_LEFT: {
      // L command - Move RA stepper left
      if (argIndex == 1) {
      //tracking = 0;
      stepperRA.move(-args[0]);
      //stepperRA.runToPosition();
      //tracking = 1;
      }
      }
      break;

      case MOVE_DOWN: {
      // DOWN command - Move DEC stepper down
      if (argIndex == 1) {
      stepperDEC.setMaxSpeed(250);
      stepperDEC.setAcceleration(600);
      stepperDEC.move(args[0]);
      //stepperDEC.runToPosition();
      }
      }
      break;

      case MOVE_UP: {
      // UP command - Move DEC stepper up
      if (argIndex == 1) {
      stepperDEC.setMaxSpeed(250);
      stepperDEC.setAcceleration(600);
      stepperDEC.move(-args[0]);
      //stepperDEC.runToPosition();
      }
      }
      break;

      default:
      break;
      }
      }

      // I command - toggle pulse guiding
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
      sprintf(current_RA, "%02d:%02d:%02d", RADisplayTime.getHours(), RADisplayTime.getMinutes(), RADisplayTime.getSeconds());
      Serial.print(current_RA);
      Serial.print("#");
      //inCmd = "";
      }

      if (inCmd == ":GD") {
      sprintf(current_DEC, "%c%02d*%02d:%02d", ' + ', int(printdegDEC), int(minDEC), int(secDEC));
      Serial.print(current_DEC);
      //Serial.print("+80*00#");
      Serial.print("#");
      //inCmd = "";
      }

      if (inCmd.indexOf(":Sr") >= 0) {
      String x = inCmd.substring(3);
      int RaH = x.toInt();
      String y = inCmd.substring(7);
      int RaM = y.toInt();
      String z = inCmd.substring(10);
      int RaS = z.toInt();

      int slew_RAh = (RaH - RADisplayTime.getHours());
      int slew_RAm = (RaM - RADisplayTime.getMinutes());
      int slew_RAs = (RaS - RADisplayTime.getSeconds());

      RATime.addTime(slew_RAh, slew_RAm, slew_RAs);

      Serial.print("1");
      slew_RA = (slew_RAh * stepsPerHour + slew_RAm * (stepsPerHour / 60) + slew_RAs * (stepsPerHour / 3600)) / 2;

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
      slew_DEC = (slew_DECd * float(DECStepsPerDegree) + slew_DECm * (float(DECStepsPerDegree) / float(60)) + slew_DECs * (float(DECStepsPerDegree) / float(3600))) / 2;

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
    */
    doCalculations();
    runTracker();
  }

  // Dont let logString grow forever. When it gets over 3K cut it down to 2K. This might wreak
  // havoc with heap fragmentation, so we might need to put the logString building
  // behind a #ifdef DEBUG since that's really only what it's needed for.
  if (logString.length() > 3000)    {
    logString = logString.substring(1000);
  }
}
