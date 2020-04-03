bool parking = false;

void startSlewing() {
  serialIsSlewing = true;
  parking = false;
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
        sprintf(scratchBuffer, "%02d:%02d:%02d#", RADisplayTime.getHours(), RADisplayTime.getMinutes(), RADisplayTime.getSeconds());
        Serial.print(scratchBuffer);
      }
      break;
    case 'd': {
        sprintf(scratchBuffer, "%c%02d*%02d'%02d#", printdegDEC > 0 ? '+' : '-', int(fabs(printdegDEC)), int(minDEC), int(secDEC));
        Serial.print(scratchBuffer);
      }
      break;
    case 'R': {
        DayTime dt(currentRA());
        dt.addTime(HACorrection);
        sprintf(scratchBuffer, "%02d:%02d:%02d#", dt.getHours(), dt.getMinutes(), dt.getSeconds());
        Serial.print(scratchBuffer);
      }
      break;

    case 'D': {
        float dec = currentDEC();
        DayTime dt(dec);
        int degree = dt.getHours() + (north ? 90 : -90);
        sprintf(scratchBuffer, "%c%02d*%02d'%02d#", degree > 0 ? '+' : '-', int(fabs(degree)), dt.getMinutes(), dt.getSeconds());
        Serial.print(scratchBuffer);
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
    Serial.print("0");
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
  if (inCmd[0] == 'P') {  // Park
    serialIsSlewing = true;
    slewingToHome = true;
    parking = true;
    moveToHomePositionsAsync();
    startMoveSteppersToTargetAsync();
  }
}

/////////////////////////////
// QUIT
/////////////////////////////
void handleMeadeQuit(String inCmd) {
  // :Q# stops a motors - remains in Control mode
  // :Qq# command does not stop motors, but quits Control mode
  if ((inCmd.length() == 0) || (inCmd[0] != 'q'))  {
    stopSteppers();
  } else {
    inSerialControl = false;
    lcdMenu.setCursor(0, 0);
    lcdMenu.updateDisplay();
  }

  serialIsSlewing = false;
}

////////////////////////////////////////////////
// The main loop when under serial control
void serialLoop()
{
  runTracker();
  if (serialIsSlewing) {
    if (!moveSteppersToTargetAsync()) {
      if (slewingToHome) {
        moveToHomePositionCompleted();
        slewingToHome = false;
        if (parking){
          tracking = 0;
        }
      }
      serialIsSlewing = false;
    }
  }
}

//////////////////////////////////////////////////
// Event that is triggered when the serial port receives data.
void serialEvent() {

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
