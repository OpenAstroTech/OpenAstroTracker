/*


  // Serial control variables
  bool serialIsSlewing = false; // When the serial port is slewing the tracker
  bool slewingToHome = false;   // When the serial port is slewing to home/park.
  bool parking = false;

  void startSlewing() {
  serialIsSlewing = true;
  parking = false;
  doCalculations();
  handleDECandRACalculations();
  startMoveSteppersToTargetAsync();
  }
*/

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
        Serial.print(mount.RAString(MEADE_STRING | TARGET_STRING));
      }
      break;
    case 'd': {
        Serial.print(mount.DECString(MEADE_STRING | TARGET_STRING));
      }
      break;
    case 'R': {
        Serial.print(mount.RAString(MEADE_STRING | CURRENT_STRING));
      }
      break;

    case 'D': {
        Serial.print(mount.DECString(MEADE_STRING | CURRENT_STRING));
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
      mount.targetDEC().set(sgn * deg + (NORTHERN_HEMISPHERE ? -90 : 90), inCmd.substring(5, 7).toInt(), inCmd.substring(8, 10).toInt());
      Serial.print("1");
    } else  {
      // Did not understand the coordinate
      Serial.print("0");
    }
  } else if (inCmd[0] == 'r') {
    // Set RA
    if ((inCmd[3] == ':') && (inCmd[6] == ':') )
    {
      mount.targetRA().set(inCmd.substring(1, 3).toInt(), inCmd.substring(4, 6).toInt(), inCmd.substring(7, 9).toInt());
      mount.targetRA().addTime(mount.getHACorrection());
      mount.targetRA().subtractTime(mount.HA());
      Serial.print("1");
    } else
      // Did not understand the coordinate
      Serial.print("0");
  } else if (inCmd[0] == 'H') {
    // Set HA
    int hHA = inCmd.substring(1, 3).toInt();
    int minHA = inCmd.substring(4, 6).toInt();
    mount.setHA(DayTime(hHA, minHA, 0));
    Serial.print("0");
  }
}

/////////////////////////////
// MOVEMENT
/////////////////////////////
void handleMeadeMovement(String inCmd) {
  if (inCmd[0] == 'S') {
    Serial.print("0");
    mount.startSlewingToTarget();
  }
}

/////////////////////////////
// PARK
/////////////////////////////
void handleMeadeHome(String inCmd) {
  if (inCmd[0] == 'P') {  // Park
    mount.setTargetToHome();
    mount.startSlewingToTarget();
    mount.waitUntilStopped(ALL_DIRECTIONS);
    mount.setHome();
    mount.stopSlewing(TRACKING);
  }
}

/////////////////////////////
// QUIT
/////////////////////////////
void handleMeadeQuit(String inCmd) {
  // :Q# stops a motors - remains in Control mode
  // :Qq# command does not stop motors, but quits Control mode
  if ((inCmd.length() == 0) || (inCmd[0] != 'q'))  {
    mount.stopSlewing(ALL_DIRECTIONS | TRACKING);
  } else {
    inSerialControl = false;
    lcdMenu.setCursor(0, 0);
    lcdMenu.updateDisplay();
    mount.startSlewing(TRACKING);
  }
}

////////////////////////////////////////////////
// The main loop when under serial control
void serialLoop()
{
  mount.loop();
  //  runTracker();
  //  if (serialIsSlewing) {
  //    if (!moveSteppersToTargetAsync()) {
  //      if (slewingToHome) {
  //        moveToHomePositionCompleted();
  //        slewingToHome = false;
  //        if (parking){
  //          tracking = 0;
  //        }
  //      }
  //      serialIsSlewing = false;
  //    }
  //  }
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
    mount.loop();
  }

  // Dont let logString grow forever. When it gets over 1K cut it down to 0.5K. This might wreak
  // havoc with heap fragmentation, so we might need to put the logString building
  // behind a #ifdef DEBUG since that's really only what it's needed for.
  if (logString.length() > 1000)    {
    logString = logString.substring(500);
  }
}
