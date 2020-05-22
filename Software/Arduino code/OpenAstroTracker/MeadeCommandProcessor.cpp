#include "MeadeCommandProcessor.h"
#include "Globals.h"
#include "Utility.h"

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
// :GX#
//      Get Mount Status
//      Returns: string reflecting the mounts' status. The string is a comma-delimited list of statuses:
//               Idle,--T,11219,0,927,071906,+900000,#
//                 |   |    |   |  |     |      |    
//                 |   |    |   |  |     |      |    
//                 |   |    |   |  |     |      |    
//                 |   |    |   |  |     |      +------------------ [6] The current DEC position
//                 |   |    |   |  |     +------------------------- [5] The current RA position
//                 |   |    |   |  +------------------------------- [4] The Tracking stepper position
//                 |   |    |   +---------------------------------- [3] The DEC stepper position
//                 |   |    +-------------------------------------- [2] The RA stepper position
//                 |   +------------------------------------------- [1] The motion state. 
//                 |                                                    First character is RA slewing state ('R' is East, 'r' is West, '-' is stopped). 
//                 |                                                    Second character is DEC slewing state ('d' is North, 'D' is South, '-' is stopped). 
//                 |                                                    Third character is TRK slewing state ('T' is Tracking, '-' is stopped). 
//                 +----------------------------------------------- [0] The mount status. One of 'Idle', 'Parked', 'Parking', 'Guiding', 'SlewToTarget', 'FreeSlew', 'ManualSlew', 'Tracking'
//
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
//      Returns: 0
//
// -- MOVEMENT Extensions --
//
// :MGdnnnn#
//      Run a Guide pulse
//      This runs the motors for a short period of time.
//      Where d is one of 'N', 'E', 'W', or 'S' and nnnn is the duration in ms.
//      Returns: 1
//
// :MTs#
//      Set Tracking mode
//      This turns the scopes tracking mode on or off.
//      Where s is 1 to turn on Tracking and 0 to turn it off.
//      Returns: 1
//
// :Mc#
//      Start slewing 
//      This starts slewing the mount in the given direction.
//      Where c is one of 'n', 'e', 'w', or 's'. 
//      Returns: nothing
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
// :hF#
//      Move Scope to Home position 
//      This slews the scope back to its home position (RA ring centered, DEC
//      at 90, basically pointing at celestial pole). Mount will keep tracking.
//      Returns: Nothing
//
// -- PARK Extensions --
// :hU#
//      Unpark Scope
//      This currently simply turns on tracking.
//      Returns: 1
//
//------------------------------------------------------------------
// QUIT MOVEMENT FAMILY
//
// :Q#
//      Stop all motors
//      This stops all motors, including tracking. Note that deceleration curves are still followed.
//      Returns: 1 when all motors have stopped.
//
// :Qd#
//      Stop slew in specified direction where d is n, s, e, w, a (the first four are the cardinal directions, a stands for All).
//      Returns: nothing
//
// -- QUIT MOVEMENT Extensions --
// :Qq#
//      Disconnect, Quit Control mode
//      This quits Serial Control mode and starts tracking.
//      Returns: nothing
//
//------------------------------------------------------------------
// EXTRA OAT FAMILY - These are meant for the PC control app
//
// :XDnnn#
//      Run drift alignment
//      This runs a drift alignment procedure where the mounts slews east, pauses, slews west and pauses.
//      Where nnn is the number of seconds the entire alignment should take.
//      Returns: nothing
//
// :XGR#
//      Get RA steps 
//      Get the number of steps the RA stepper motor needs to take to rotate by one degree 
//      Returns: integer
//
// :XGD#
//      Get DEC steps 
//      Get the number of steps the DEC stepper motor needs to take to rotate by one degree 
//      Returns: integer
//
// :XGS#
//      Get Tracking speed adjustment
//      Get the adjustment factor used to speed up (>1.0) or slow down (<1.0) the tracking speed of the mount.
//      Returns: float
//
// :XGH#
//      Get HA
//      Get the current HA of the mount.
//      Returns: HHMMSS
//
// :XGL#
//      Get LST
//      Get the current LST of the mount.
//      Returns: HHMMSS
//
// :XSRn#
//      Set RA steps 
//      Set the number of steps the RA stepper motor needs to take to rotate by one degree 
//      Where n is the number of steps
//      Returns: nothing
//
// :XSDn#
//      Set DEC steps 
//      Set the number of steps the DEC stepper motor needs to take to rotate by one degree 
//      Where n is the number of steps
//      Returns: nothing
//
// :XSSn.n#
//      Set Tracking speed adjustment
//      Set the adjustment factor used to speed up (>1.0) or slow down (<1.0) the tracking speed of the mount.
//      Returns: nothing
//
// :XSMn#
//      Set Manual Slewing Mode
//      Toggle the manual slewing mode state where the RA and DEC motors run at a constant speed.
//      Where n is '1' to turn it on, otherwise turn it off.
//      Returns: nothing
//
// :XSXn.nnn#
//      Set RA manual slewing speed
//      Set the speed of the RA motor, immediately. Must be in manual slewing mode
//      Returns: nothing
//
// :XSYn.nnn#
//      Set DEC manual slewing speed
//      Set the speed of the DEC motor, immediately.
//      Returns: nothing
//
/////////////////////////////////////////////////////////////////////////////////////////

MeadeCommandProcessor* MeadeCommandProcessor::_instance = nullptr;

/////////////////////////////
// Create the processor 
/////////////////////////////
MeadeCommandProcessor* MeadeCommandProcessor::createProcessor(Mount* mount, LcdMenu* lcdMenu) {
  _instance = new MeadeCommandProcessor(mount, lcdMenu);
  return _instance;
}

/////////////////////////////
// Get the singleton
/////////////////////////////
MeadeCommandProcessor* MeadeCommandProcessor::instance() {
  return _instance;
}

/////////////////////////////
// Constructor 
/////////////////////////////
MeadeCommandProcessor::MeadeCommandProcessor(Mount* mount, LcdMenu* lcdMenu) {
  _mount = mount;

  // In HEADLESS_CLIENT, the lcdMenu is just an empty shell class to save having to null check everywhere
  _lcdMenu = lcdMenu;
}

/////////////////////////////
// INIT
/////////////////////////////
String MeadeCommandProcessor::handleMeadeInit(String inCmd) {
  inSerialControl = true;
  _lcdMenu->setCursor(0, 0);
  _lcdMenu->printMenu("Remote control");
  _lcdMenu->setCursor(0, 1);
  _lcdMenu->printMenu(">SELECT to quit");
  return "";
}

/////////////////////////////
// GET INFO
/////////////////////////////
String MeadeCommandProcessor::handleMeadeGetInfo(String inCmd) {
  char cmdOne = inCmd[0];
  char cmdTwo = (inCmd.length() > 1) ? inCmd[1] : '\0';

  switch (cmdOne) {
    case 'V':
    if (cmdTwo == 'N') {
      return version + "#";
    }
    else if (cmdTwo == 'P') {
      return "OpenAstroTracker#";
    }
    break;

    case 'r': return _mount->RAString(MEADE_STRING | TARGET_STRING);

    case 'd': return _mount->DECString(MEADE_STRING | TARGET_STRING);

    case 'R': return _mount->RAString(MEADE_STRING | CURRENT_STRING);

    case 'D': return _mount->DECString(MEADE_STRING | CURRENT_STRING);

    case 'X': return _mount->getStatusString() + "#";

    case 'I':
    String retVal = "";
    if (cmdTwo == 'S') {
      retVal = _mount->isSlewingRAorDEC() ? "1" : "0";
    }
    else if (cmdTwo == 'T') {
      retVal = _mount->isSlewingTRK() ? "1" : "0";
    }
    else if (cmdTwo == 'G') {
      retVal = _mount->isGuiding() ? "1" : "0";
    }
    return retVal + "#";
  }

  return "0#";
}

/////////////////////////////
// SYNC CONTROL
/////////////////////////////
String MeadeCommandProcessor::handleMeadeSyncControl(String inCmd) {
  if (inCmd[0] == 'M') {
    _mount->syncDEC(_mount->targetDEC().getHours(), _mount->targetDEC().getMinutes(), _mount->targetDEC().getSeconds());
    _mount->syncRA(_mount->targetRA().getHours(), _mount->targetRA().getMinutes(), _mount->targetRA().getSeconds());
    return "NONE#";
  }

  return "0";

}

/////////////////////////////
// SET INFO
/////////////////////////////
String MeadeCommandProcessor::handleMeadeSetInfo(String inCmd) {
  if ((inCmd[0] == 'd') && (inCmd.length() == 10)) {
    // Set DEC
    //   0123456789
    // :Sd+84*03:02
    int sgn = inCmd[1] == '+' ? 1 : -1;
    if ((inCmd[4] == '*') && (inCmd[7] == ':'))
    {
      int deg = inCmd.substring(2, 4).toInt();
      _mount->targetDEC().set(sgn * deg + (NORTHERN_HEMISPHERE ? -90 : 90), inCmd.substring(5, 7).toInt(), inCmd.substring(8, 10).toInt());
      return "1";
    }
    else {
      // Did not understand the coordinate
      return "0";
    }
  }
  else if (inCmd[0] == 'r' && (inCmd.length() == 9)) {
    // :Sr11:04:57#
    // Set RA
    //   012345678
    // :Sr04:03:02
    if ((inCmd[3] == ':') && (inCmd[6] == ':'))
    {
      _mount->targetRA().set(inCmd.substring(1, 3).toInt(), inCmd.substring(4, 6).toInt(), inCmd.substring(7, 9).toInt());
      return "1";
    }
    else {
      // Did not understand the coordinate
      return "0";
    }
  }
  else if (inCmd[0] == 'H') {
    if (inCmd[1] == 'L') {
      // Set LST
      int hLST = inCmd.substring(2, 4).toInt();
      int minLST = inCmd.substring(4, 6).toInt();
      int secLST = 0;
      if (inCmd.length() > 7) {
        secLST = inCmd.substring(6, 8).toInt();
      }

      DayTime lst(hLST, minLST, secLST);
#ifdef DEBUG_MODE
      logv("MeadeSetInfo: Received LST: %d:%d:%d", hLST,minLST,secLST);
#endif
      _mount->setLST(lst);
    }
    else if (inCmd[1] == 'P') {
      // Set home point
      _mount->setHome();
      _mount->startSlewing(TRACKING);
    }
    else {
      // Set HA
      int hHA = inCmd.substring(1, 3).toInt();
      int minHA = inCmd.substring(4, 6).toInt();
#ifdef DEBUG_MODE
      logv("MeadeSetInfo: Received HA: %d:%d:%d", hHA, minHA, 0);
#endif
      _mount->setHA(DayTime(hHA, minHA, 0));
    }

    return "1";
  }
  else if ((inCmd[0] == 'Y') && inCmd.length() == 19) {
    // Sync RA, DEC - current position is teh given coordinate
    //   0123456789012345678
    // :SY+84*03:02.18:34:12
    int sgn = inCmd[1] == '+' ? 1 : -1;
    if ((inCmd[4] == '*') && (inCmd[7] == ':') && (inCmd[10] == '.') && (inCmd[13] == ':') && (inCmd[16] == ':')) {
      int deg = inCmd.substring(2, 4).toInt();
      _mount->syncDEC(sgn * deg + (NORTHERN_HEMISPHERE ? -90 : 90), inCmd.substring(5, 7).toInt(), inCmd.substring(8, 10).toInt());
      _mount->syncRA(inCmd.substring(11, 13).toInt(), inCmd.substring(14, 16).toInt(), inCmd.substring(17, 19).toInt());
      return "1";
    }
    else {
      return "0";
    }
  }
  else if ((inCmd[0] == 't')) // longitude: :St+30*29#
  {
    return "1";
  }
  else if (inCmd[0] == 'g') // latitude :Sg097*34#
  {
    return "1";
  }
  else if (inCmd[0] == 'G') // utc offset :SG+05#
  {
    return "1";
  }
  else if (inCmd[0] == 'L') // Local time :SL19:33:03#
  {
    return "1";
  }
  else if (inCmd[0] == 'C') { // Set Date (MM/DD/YY) :SC04/30/20#
    return "1Updating Planetary Data#"; // 
  }
  else {
    return "0";
  }
}

/////////////////////////////
// MOVEMENT
/////////////////////////////
String MeadeCommandProcessor::handleMeadeMovement(String inCmd) {
  if (inCmd[0] == 'S') {
    _mount->startSlewingToTarget();
    return "0";
  }
  else if (inCmd[0] == 'T') {
    if (inCmd.length() > 1) {
      if (inCmd[1] == '1') {
        _mount->startSlewing(TRACKING);
        return "1";
      }
      else if (inCmd[1] == '0') {
        _mount->stopSlewing(TRACKING);
        return "1";
      }
    }
    else {
      return "0";
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
      _mount->guidePulse(direction, duration);
      return "1";
    }
  }
  else if (inCmd[0] == 'e') {
    _mount->startSlewing(EAST);
    return "";
  }
  else if (inCmd[0] == 'w') {
    _mount->startSlewing(WEST);
    return "";
  }
  else if (inCmd[0] == 'n') {
    _mount->startSlewing(NORTH);
    return "";
  }
  else if (inCmd[0] == 's') {
    _mount->startSlewing(SOUTH);
    return "";
  }

  return "0";
}

/////////////////////////////
// HOME
/////////////////////////////
String MeadeCommandProcessor::handleMeadeHome(String inCmd) {
  if (inCmd[0] == 'P') {  // Park
    _mount->park();
  }
  else if (inCmd[0] == 'F') {  // Home
    _mount->goHome(true);
  }
  else if (inCmd[0] == 'U') {  // Unpark
    _mount->startSlewing(TRACKING);
    return "1";
  }
  return "";
}

/////////////////////////////
// EXTRA COMMANDS
/////////////////////////////
String MeadeCommandProcessor::handleMeadeExtraCommands(String inCmd) {
  //   0123
  // :XDmmm
  if (inCmd[0] == 'D') {  // Drift Alignemnt
    int duration = inCmd.substring(1, 4).toInt() - 3;
    _lcdMenu->setCursor(0, 0);
    _lcdMenu->printMenu(">Drift Alignment");
    _lcdMenu->setCursor(0, 1);
    _lcdMenu->printMenu("Pause 1.5s....");
    _mount->stopSlewing(ALL_DIRECTIONS | TRACKING);
    _mount->waitUntilStopped(ALL_DIRECTIONS);
    _mount->delay(1500);
    _lcdMenu->setCursor(0, 1);
    _lcdMenu->printMenu("Eastward pass...");
    _mount->runDriftAlignmentPhase(EAST, duration);
    _lcdMenu->setCursor(0, 1);
    _lcdMenu->printMenu("Pause 1.5s....");
    _mount->delay(1500);
    _lcdMenu->printMenu("Westward pass...");
    _mount->runDriftAlignmentPhase(WEST, duration);
    _lcdMenu->setCursor(0, 1);
    _lcdMenu->printMenu("Pause 1.5s....");
    _mount->delay(1500);
    _lcdMenu->printMenu("Reset _mount->..");
    _mount->runDriftAlignmentPhase(0, duration);
    _lcdMenu->setCursor(0, 1);
    _mount->startSlewing(TRACKING);
  }
  else if (inCmd[0] == 'G') { // Get RA/DEC steps/deg, speedfactor
    if (inCmd[1] == 'R') {
      return String(_mount->getStepsPerDegree(RA_STEPS)) + "#";
    }
    else if (inCmd[1] == 'D') {
      return String(_mount->getStepsPerDegree(DEC_STEPS)) + "#";
    }
    else if (inCmd[1] == 'S') {
      return String(_mount->getSpeedCalibration(), 5) + "#";
    }
    else if (inCmd[1] == 'H') {
      char scratchBuffer[10];
      sprintf(scratchBuffer, "%02d%02d%02d#", _mount->HA().getHours(), _mount->HA().getMinutes(), _mount->HA().getSeconds());
      return String(scratchBuffer);
    }
    else if (inCmd[1] == 'L') {
      char scratchBuffer[10];
      sprintf(scratchBuffer, "%02d%02d%02d#", _mount->LST().getHours(), _mount->LST().getMinutes(), _mount->LST().getSeconds());
      return String(scratchBuffer);
    }
  }
  else if (inCmd[0] == 'S') { // Set RA/DEC steps/deg, speedfactor
    if (inCmd[1] == 'R') {
      _mount->setStepsPerDegree(RA_STEPS, inCmd.substring(2).toInt());
    }
    else if (inCmd[1] == 'D') {
      _mount->setStepsPerDegree(DEC_STEPS, inCmd.substring(2).toInt());
    }
    else if (inCmd[1] == 'S') {
      _mount->setSpeedCalibration(inCmd.substring(2).toFloat(), true);
    }
    else if (inCmd[1] == 'M') {
      _mount->setManualSlewMode(inCmd[2] == '1');
    }
    else if (inCmd[1] == 'X') {
      _mount->setSpeed(RA_STEPS, inCmd.substring(2).toFloat());
    }
    else if (inCmd[1] == 'Y') {
      _mount->setSpeed(DEC_STEPS, inCmd.substring(2).toFloat());
    }
  }
  return "";
}


/////////////////////////////
// QUIT
/////////////////////////////
String MeadeCommandProcessor::handleMeadeQuit(String inCmd) {
  // :Q# stops a motors - remains in Control mode
  // :Qq# command does not stop motors, but quits Control mode
  if (inCmd.length() == 0) {
    _mount->stopSlewing(ALL_DIRECTIONS | TRACKING);
    _mount->waitUntilStopped(ALL_DIRECTIONS);
    return "1";
  }

  switch (inCmd[0]) {
    case 'a':
      _mount->stopSlewing(ALL_DIRECTIONS);
    break;
    case 'e':
      _mount->stopSlewing(EAST);
      break;
  case 'w':
      _mount->stopSlewing(WEST);
      break;
  case 'n':
      _mount->stopSlewing(NORTH);
      break;
  case 's':
      _mount->stopSlewing(SOUTH);
      break;
  case 'q':
      inSerialControl = false;
      _lcdMenu->setCursor(0, 0);
      _lcdMenu->updateDisplay();
      break;
  }

  return "";
}

/////////////////////////////
// Set Slew Rates
/////////////////////////////
String MeadeCommandProcessor::handleMeadeSetSlewRate(String inCmd) {
  switch (inCmd[1]) {
    case 'S': // Slew   - Fastest
    case 'M': // Find   - 2nd Fastest
    case 'C': // Center - 2nd Slowest
    case 'G': // Guide  - Slowest
    default:
    return "";
  }
}

String MeadeCommandProcessor::processCommand(String inCmd) {
  if (inCmd[0] = ':') {
    char command = inCmd[1];
    inCmd = inCmd.substring(2);
    switch (command) {
      case 'S': return handleMeadeSetInfo(inCmd);
      case 'M': return handleMeadeMovement(inCmd);
      case 'G': return handleMeadeGetInfo(inCmd);
      case 'C': return handleMeadeSyncControl(inCmd);
      case 'h': return handleMeadeHome(inCmd);
      case 'I': return handleMeadeInit(inCmd);
      case 'Q': return handleMeadeQuit(inCmd);
      case 'R': return handleMeadeSetSlewRate(inCmd);
      case 'X': return handleMeadeExtraCommands(inCmd);
      default:
#ifdef DEBUG_MODE
      Serial.println("Unknown Command in MeadeCommandProcessor::processCommand " + inCmd);
      return "";
#endif
      break;
    }
  }
}
