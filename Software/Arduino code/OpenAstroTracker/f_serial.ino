#ifdef SUPPORT_SERIAL_CONTROL

/////////////////////////////////////////////////////////////////////////////////////////
//
// Serial support
//
// The Serial protocol implemented her is the Meade protocol with some extensions.
// The Meade protocol commands start with a colon and end with a hash.
// The first letter determines the family of functions (G for Get, S for Set, M for Movement, etc.)
//
// The set of Meade features implemented are:
//
//------------------------------------------------------------------
// INITIALIZE FAMILY
//
// :I#   - Initialize Scope
//         This puts the Arduino in Serial Control Mode and displays RA on line 1 and
//         DEC on line 2 of the display. Serial Control Mode can be ended manually by
//         pressing the SELECT key, or programmatically with the :Qq# command.
//         Returns: nothing
//
//------------------------------------------------------------------
// SYNC CONTROL FAMILY
//
// :CM#
//      Synchronize Declination and Right Ascension.
//      This tells the scope what it is currently pointing at.
//      The scope synchronizes to the current target coordinates (set with :Sd# and :Sr#)
//      Returns: NONE#
//
//------------------------------------------------------------------
// GET FAMILY
//
// :GVP#
//      Get the Product Name
//      Returns: 'OpenAstroTracker'
//
// :GVN#
//      Get the Firmware Version Number
//      Returns: 'V1.major.minor' from OpenAstroTracker.ino
//
// :Gd#
//      Get Target Declination
//      Where s is + or -, DD is degrees, MM is minutes, SS is seconds.
//      Returns: sDD*MM'SS
//
// :GD#
//      Get Current Declination
//      Where s is + or -, DD is degrees, MM is minutes, SS is seconds.
//      Returns: sDD*MM'SS
//
// :Gr#
//      Get Target Right Ascension
//      Where HH is hour, MM is minutes, SS is seconds.
//      Returns: HH:MM:SS
//
// :GR#
//      Get Current Right Ascension
//      Where HH is hour, MM is minutes, SS is seconds.
//      Returns: HH:MM:SS
//
// -- GET Extensions --
// :GIS#
//      Get DEC or RA Slewing
//      Returns: 1 if either RA or DEC is slewing. 0 if not.
//
// :GIT#
//      Get Tracking
//      Returns: 1 if tracking is on. 0 if not.
//
// :GIG#
//      Get Guiding
//      Returns: 1 if currently guiding. 0 if not.
//
//------------------------------------------------------------------
// SET FAMILY
//
// :SdsDD*MM:SS#
//      Set Target Declination
//      This sets the target DEC. Use a Movement command to slew there.
//      Where s is + or -, DD is degrees, MM is minutes, SS is seconds.
//      Returns: 1 if successfully set, otherwise 0
//
// :SrHH:MM:SS#
//      Set Right Ascension
//      This sets the target RA. Use a Movement command to slew there.
//      Where HH is hours, MM is minutes, SS is seconds.
//      Returns: 1 if successfully set, otherwise 0
//
// -- SET Extensions --
// :SHHH:MM#
//      Set Hour Time (HA)
//      This sets the scopes HA.
//      Where HH is hours, MM is minutes.
//      Returns: 1 if successfully set, otherwise 0
//
// :SYsDD*MM:SS.HH:MM:SS#
//      Synchronize Declination and Right Ascension.
//      This tells the scope what it is currently pointing at.
//      Where s is + or -, DD is degrees, HH is hours, MM is minutes, SS is seconds.
//      Returns: 1 if successfully set, otherwise 0
//
//------------------------------------------------------------------
// MOVEMENT FAMILY
//
// :MS#
//      Start Slew to Target (Asynchronously)
//      This starts slewing the scope to the target RA and DEC coordinates and returns immediately.
//      Returns: 1
//
// -- MOVEMENT Extensions --
//
// :MGdnnnn#
//      Run a Guide pulse
//      This runs the motors for a short period of time.
//      Where d is one of 'N', 'E', 'W', or 'S' and nnnn is the duration in ms.
//      Returns: nothing
//
// :MTs#
//      Set Tracking mode
//      This turns the scopes tracking mode on or off.
//      Where s is 1 to turn on Tracking and 0 to turn it off.
//      Returns: 1
//
//------------------------------------------------------------------
// HOME FAMILY
//
// :hP#
//      Park Scope and stop motors
//      This slews the scope back to it's home position (RA ring centered, DEC
//      at 90, basically pointing at celestial pole) and stops all movement (including tracking).
//      Returns: Nothing
//
// -- PARK Extensions --
// :hU#
//      Unpark Scope
//      This currently simply turns on tracking.
//      Returns: Nothing
//
//------------------------------------------------------------------
// QUIT MOVEMENT FAMILY
//
// :Q#
//      Stop all motors
//      This stops all motors, including tracking. Note that deceleration curves are still followed.
//      Returns: 1 when all motors have stopped.
//
// -- QUIT MOVEMENT Extensions --
// :Qq#
//      Disconnect, Quit Control mode
//      This quits Serial Control mode and starts tracking.
//      Returns: nothing
//
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////
// INIT
/////////////////////////////
void handleMeadeInit(String inCmd) {
  inSerialControl = true;
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

    case 'I': {
        if (cmdTwo == 'S') {
          Serial.print(mount.isSlewingRAorDEC() ? "1" : "0");
        }
        else if (cmdTwo == 'T') {
          Serial.print(mount.isSlewingTRK() ? "1" : "0");
        }
        else if (cmdTwo == 'G') {
          Serial.print(mount.isGuiding() ? "1" : "0");
        }
        Serial.print("#");
      }
      break;
  }
}

/////////////////////////////
// SYNC CONTROL
/////////////////////////////
void handleMeadeSyncControl(String inCmd) {
  if (inCmd[0] == 'M') {
    mount.syncDEC(mount.targetDEC().getHours(), mount.targetDEC().getMinutes(), mount.targetDEC().getSeconds());
    mount.syncRA(mount.targetRA().getHours(), mount.targetRA().getMinutes(), mount.targetRA().getSeconds());
    Serial.print("NONE#");
  }
  else {
    Serial.print("0");
  }
}

/////////////////////////////
// SET INFO
/////////////////////////////
void handleMeadeSetInfo(String inCmd) {
  if (inCmd.length() < 6) {
    Serial.print("0");
    return;
  }
  if ((inCmd[0] == 'd') && (inCmd.length() == 10)) {
    // Set DEC
    //   0123456789
    // :Sd+84*03:02
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
  } else if (inCmd[0] == 'r' && (inCmd.length() == 9)) {
    // Set RA
    //   012345678
    // :Sr04:03:02
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
    Serial.print("1");
  }
  else if ((inCmd[0] == 'Y') && inCmd.length() == 19) {
    // Sync RA, DEC - current position is teh given coordinate
    //   0123456789012345678
    // :SY+84*03:02.18:34:12
    int sgn = inCmd[1] == '+' ? 1 : -1;
    if ((inCmd[4] == '*') && (inCmd[7] == ':') && (inCmd[10] == '.') && (inCmd[13] == ':') && (inCmd[16] == ':')) {
      int deg = inCmd.substring(2, 4).toInt();
      mount.syncDEC(sgn * deg + (NORTHERN_HEMISPHERE ? -90 : 90), inCmd.substring(5, 7).toInt(), inCmd.substring(8, 10).toInt());
      mount.syncRA(inCmd.substring(11, 13).toInt(), inCmd.substring(14, 16).toInt(), inCmd.substring(17, 19).toInt());
      Serial.print("1");
    }
    else {
      Serial.print("0");
    }
  }
  else {
    Serial.print("0");
  }
}

/////////////////////////////
// MOVEMENT
/////////////////////////////
void handleMeadeMovement(String inCmd) {
  if (inCmd[0] == 'S') {
    mount.startSlewingToTarget();
    Serial.print("1");
  }
  else if (inCmd[0] == 'T') {
    if (inCmd.length() > 1) {
      if (inCmd[1] == '1') {
        mount.startSlewing(TRACKING);
        Serial.print("1");
      }
      else if (inCmd[1] == '0')    {
        mount.stopSlewing(TRACKING);
        Serial.print("1");
      }
    }
    else {
      Serial.print("0");
    }
  }
  else if (inCmd[0] == 'G') {
    // Guide pulse
    //   012345678901
    // :MGd0403
    if (inCmd.length() == 6) {
      byte direction = EAST;
      if (inCmd[1] == 'N') direction = NORTH;
      else if (inCmd[1] == 'S') direction = SOUTH;
      else if (inCmd[1] == 'E') direction = EAST;
      else if (inCmd[1] == 'W') direction = WEST;
      int duration = (inCmd[2] - '0') * 1000 + (inCmd[3] - '0') * 100 + (inCmd[4] - '0') * 10 + (inCmd[5] - '0');
      mount.guidePulse(direction, duration);
    }
  }
  else if (inCmd[0] == 'e') {
    mount.startSlewing(EAST);
  }
  else if (inCmd[0] == 'w') {
    mount.startSlewing(WEST);
  }
  else if (inCmd[0] == 'n') {
    mount.startSlewing(NORTH);
  }
  else if (inCmd[0] == 's') {
    mount.startSlewing(SOUTH);
  }
}

/////////////////////////////
// HOME
/////////////////////////////
void handleMeadeHome(String inCmd) {
  if (inCmd[0] == 'P') {  // Park
    mount.park();
  }
  else if (inCmd[0] == 'U') {  // Unpark
    mount.startSlewing(TRACKING);
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
    mount.waitUntilStopped(ALL_DIRECTIONS);
    Serial.print("1");
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
  mount.displayStepperPositionThrottled();
}

//////////////////////////////////////////////////
// Event that is triggered when the serial port receives data.
void serialEvent() {

  // Implement Meade protocol
  while (Serial.available() > 0) {

    String inCmd = Serial.readStringUntil('#');

    if (inCmd[0] = ':')
    {

      char command = inCmd[1];
      inCmd = inCmd.substring(2);
      switch (command) {
        case 'S' : handleMeadeSetInfo(inCmd); break;
        case 'M' : handleMeadeMovement(inCmd); break;
        case 'G' : handleMeadeGetInfo(inCmd); break;
        case 'C' : handleMeadeSyncControl(inCmd); break;
        case 'h' : handleMeadeHome(inCmd); break;
        case 'I' : handleMeadeInit(inCmd); break;
        case 'Q' : handleMeadeQuit(inCmd); break;
      }
    }

    mount.loop();
  }
}

#endif
