#include "MeadeCommandProcessor.hpp"
#include "inc/Config.hpp"
#include "inc/Globals.hpp"
#include "Utility.hpp"
#include "WifiControl.hpp"
#include "Gyro.hpp"

#if USE_GPS == 1
bool gpsAqcuisitionComplete(int & indicator); // defined in c72_menuHA_GPS.hpp
#endif
/////////////////////////////////////////////////////////////////////////////////////////
//
// Serial support
//
// The Serial protocol implemented here is the Meade LX200 Classic protocol with some extensions.
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
// DISTANCE FAMILY
//
// :D#
//      Returns slewing status
//      Returns: '|#' if slewing, ' #' if not
//
//------------------------------------------------------------------
// GPS FAMILY
//
// :gT#
//      Attempts to set the mount time and location from the GPS for 2 minutes. This is essentially a
//      blocking call, no other activities take place (except tracking, but only if interrupt-driven).
//      Use :Gt# and :Gg# to retrieve Lat and Long,
//      Returns: 1 if the data was set, 0 if not (timedout)
//
// :gTnnn#
//      Attempts to set the mount time and location from the GPS with a custom timeout. This is also blocking
//      but by using a low timeout, you can avoid long pauses and let the user know that it's not ready yet.
//      Where nnn is an integer defining the number of milliseconds to wait for the GPS to get a bearing.
//      Returns: 1 if the data was set, 0 if not (timedout)
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
//      Returns: sDD*MM'SS
//               Where s is + or -, DD is degrees, MM is minutes, SS is seconds.
//
// :GD#
//      Get Current Declination
//      Returns: sDD*MM'SS
//               Where s is + or -, DD is degrees, MM is minutes, SS is seconds.
//
// :Gr#
//      Get Target Right Ascension
//      Returns: HH:MM:SS
//               Where HH is hour, MM is minutes, SS is seconds.
//
// :GR#
//      Get Current Right Ascension
//      Returns: HH:MM:SS
//               Where HH is hour, MM is minutes, SS is seconds.
//
// :Gt#
//      Get Site Latitude
//      Returns: sDD*MM
//               Where s is + or - and DD is the latitude in degrees and MM the minutes.
//       
// :Gg#
//      Get Site Longitude
//      Returns: DDD*MM
//               Where DDD is the longitude in degrees and MM the minutes. Negative (W) longitudes have had 360 added to them.
//       
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
//               Idle,--T--,11219,0,927,071906,+900000,#
//                 |    |     |   |  |     |      |    
//                 |    |     |   |  |     |      |    
//                 |    |     |   |  |     |      |    
//                 |    |     |   |  |     |      +------------------ [6] The current DEC position
//                 |    |     |   |  |     +------------------------- [5] The current RA position
//                 |    |     |   |  +------------------------------- [4] The Tracking stepper position
//                 |    |     |   +---------------------------------- [3] The DEC stepper position
//                 |    |     +-------------------------------------- [2] The RA stepper position
//                 |    +-------------------------------------------- [1] The motion state. 
//                 |                                                      First character is RA slewing state ('R' is East, 'r' is West, '-' is stopped). 
//                 |                                                      Second character is DEC slewing state ('d' is North, 'D' is South, '-' is stopped). 
//                 |                                                      Third character is TRK slewing state ('T' is Tracking, '-' is stopped). 
//                 |                                                      * Fourth character is AZ slewing state ('Z' and 'z' is adjusting, '-' is stopped). 
//                 |                                                      * Fifth character is ALT slewing state ('A' and 'a' is adjusting, '-' is stopped). 
//                 +------------------------------------------------- [0] The mount status. One of 'Idle', 'Parked', 'Parking', 'Guiding', 'SlewToTarget', 'FreeSlew', 'ManualSlew', 'Tracking', 'Homing'
//
//       * Az and Alt are optional. The string may only be 3 characters long
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
// :StsDD*MM#
//      Set Site Latitude
//      This sets the latitude of the location of the mount.
//      Where s is the sign ('+' or '-'), DD is the degree (90 or less), MM is minutes.
//      Returns: 1 if successfully set, otherwise 0
//
// :SgDDD*MM#
//      Set Site Longitude
//      This sets the longitude of the location of the mount.
//      Where DDD the nmber of degrees (0 to 360), MM is minutes. W Latitudes get 360 added to them. So W122 (or -122) would be 238.
//      Returns: 1 if successfully set, otherwise 0
//
// :SGsHH#
//      Set Site UTC Offset
//      This sets the offset of the timezone in which the mount is in hours from UTC.
//      Where s is the sign and HH is the number of hours.
//      CURRENTLY IGNORED.
//      Returns: 1 
//
// :SLHH:MM:SS#
//      Set Site Local Time
//      This sets the local time of the timezone in which the mount is located.
//      Where HH is hours, MM is minutes and SS is seconds.
//      CURRENTLY IGNORED.
//      Returns: 1 
//
// :SCMM/DD/YY#
//      Set Site Date
//      This sets the date
//      Where HHMM is the month, DD is teh day and YY is the year since 2000.
//      CURRENTLY IGNORED.
//      Returns: 1Updating Planetary Data 
//
// -- SET Extensions --
// :SHHH:MM#
//      Set Hour Time (HA)
//      This sets the scopes HA.
//      Where HH is hours, MM is minutes.
//      Returns: 1 if successfully set, otherwise 0
//
// :SHP#
//      Set Home Point
//      This sets the current orientation of the scope as its home point.
//      Returns: 1 
//
// :SHLHH:MM#
//      Set LST Time 
//      This sets the scopes LST (and HA).
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
// RATE CONTROL FAMILY
//
// :Rs#
//      Set Slew rate
//      Where s is one of 'S', 'M', 'C', or 'G' in order of decreasing speed
//      Returns: nothing
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
//      This runs the motors at increased speed for a short period of time.
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
// :MAZn.nn#
//      Move Azimuth 
//      If the scope supports automated azimuth operation, move azimuth by n.nn arcminutes
//      Where n.nn is a signed floating point number representing the number of arcminutes to move the mount left or right.
//      Returns: nothing
//
// :MALn.nn#
//      Move Altitude
//      If the scope supports automated altitude operation, move altitude by n.nn arcminutes
//      Where n.nn is a signed floating point number representing the number of arcminutes to raise or lower the mount.
//      Returns: nothing
//
//------------------------------------------------------------------
// SYNC FAMILY
//
// :CM#
//      Synchronizes the mount to the current target RA and DEC values.
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
// :XL0#
//      Turn off the Digital level
//      Returns: 1# or 0# if there is no Digital Level
//
// :XL1#
//      Turn on the Digital level
//      Returns: 1# or 0# if there is no Digital Level
//
// :XLGR#
//      Get Reference pitch and roll values (Digital Level)
//      Returns: <pitch>,<roll># or 0# if there is no Digital Level
//
// :XLGC#
//      Get current pitch and roll values (Digital Level)
//      Returns: <pitch>,<roll># or 0# if there is no Digital Level
//
// :XLSR#
//      Set Reference roll (Digital Level)
//      Returns: 1# or 0# if there is no Digital Level
//
// :XLSP#
//      Set Reference pitch (Digital Level)
//      Returns: 1# or 0# if there is no Digital Level
//
// :XGB#
//      Get Backlash correction steps 
//      Get the number of steps the RA stepper motor needs to overshoot and backtrack when slewing east.
//      Returns: integer
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
// :XGT#
//      Get Tracking speed
//      Get the absolute tracking speed of the mount.
//      Returns: float
//
// :XGH#
//      Get HA
//      Get the current HA of the mount.
//      Returns: HHMMSS
//
// :XGM#
//      Get Mount configuration settings 
//      Returns: <board>,<RA Stepper Info>,<DEC Stepper Info>,<GPS info>,<AzAlt info>,<Gyro info>#
//      Where <board> is one of the supported boards (currently Uno, Mega, ESP8266, ESP32)
//            <Stepper Info> is a pipe-delimited string of Motor type (NEMA or 28BYJ), Pulley Teeth, Steps per revolution)
//            <GPS info> is either NO_GPS or GPS, depending on whether a GPS module is present
//            <AzAlt info> is either NO_AZ_ALT or AUTO_AZ_ALT, depending on whether the AutoPA stepper motors are present
//            <Gyro info> is either NO_GYRO or GYRO depending on whether the Digial level is present
//      Example: ESP32,28BYJ|16|4096.00,28BYJ|16|4096.00,NO_GPS,NO_AZ_ALT,NO_GYRO#
//
// :XGN#
//      Get network settings
//      Gets the current status of the Wifi connection. Reply only available when running on ESP8266 boards.
//      Returns: 1,<stats>,<hostname>,<ip>:<port>,<SSID>,<OATHostname>#     - if Wifi is enabled
//      Returns: 0,#     - if Wifi is not enabled
//
// :XGL#
//      Get LST
//      Get the current LST of the mount.
//      Returns: HHMMSS
//
// :XSBn#
//      Set Backlash correction steps 
//      Sets the number of steps the RA stepper motor needs to overshoot and backtrack when slewing east.
//      Returns: nothing
//
// :XSRn#
//      Set RA steps 
//      Set the number of steps the RA stepper motor needs to take to rotate by one degree.
//      Where n is the number of steps
//      Returns: nothing
//
// :XSDn#
//      Set DEC steps 
//      Set the number of steps the DEC stepper motor needs to take to rotate by one degree.
//      Where n is the number of steps
//      Returns: nothing
//
// :XSSn.nnn#
//      Set Tracking speed adjustment
//      Set the adjustment factor used to speed up (>1.0) or slow down (<1.0) the tracking speed of the mount.
//      Where n.nnn is the factor to multiply the theoretical speed by.
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
//      Set the speed of the RA motor, immediately. Must be in manual slewing mode.
//      Returns: nothing
//
// :XSYn.nnn#
//      Set DEC manual slewing speed
//      Set the speed of the DEC motor, immediately. Must be in manual slewing mode.
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

  // In HEADLESS_CLIENT mode, the lcdMenu is just an empty shell class to save having to null check everywhere
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
      return String(VERSION) + "#";
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
    {
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
    case 't': {
      auto lat = DegreeTime(_mount->latitude());
      char achBuffer[20];
      sprintf(achBuffer, "%c%02d*%02d#", lat.getTotalDegrees() >= 0 ? '+' : '-', int(fabs(lat.getDegrees())), lat.getMinutes());
      return String(achBuffer);
    }
    case 'g': {
      float lon = _mount->longitude();
      if (lon < 0) {
        lon += 360;
      }
      int lonMin = (lon - (int)lon) * 60;
      char achBuffer[20];
      sprintf(achBuffer, "%03d*%02d#", (int)lon, lonMin);
      return String(achBuffer);
    }
  }

  return "0#";
}

/////////////////////////////
// GPS CONTROL
/////////////////////////////
String MeadeCommandProcessor::handleMeadeGPSCommands(String inCmd) {
  #if USE_GPS == 1
  if (inCmd[0] == 'T') {
    unsigned long timeoutLen = 2UL * 60UL * 1000UL;
    if (inCmd.length() > 1) {
      timeoutLen = inCmd.substring(1).toInt();
    }
    // Wait at most 2 minutes
    unsigned long timeoutTime = millis() + timeoutLen;
    int indicator = 0;
    while (millis() < timeoutTime) {
      if (gpsAqcuisitionComplete(indicator)) {
        LOGV1(DEBUG_MEADE, F("MEADE: GPS startup, GPS acquired"));
        return "1";
      }
    }
  }
  #endif
  LOGV1(DEBUG_MEADE, F("MEADE: GPS startup, no GPS signal"));
  return "0";
}

/////////////////////////////
// SYNC CONTROL
/////////////////////////////
String MeadeCommandProcessor::handleMeadeSyncControl(String inCmd) {
  if (inCmd[0] == 'M') {
    _mount->syncPosition(_mount->targetRA().getHours(), _mount->targetRA().getMinutes(), _mount->targetRA().getSeconds(), _mount->targetDEC().getHours(), _mount->targetDEC().getMinutes(), _mount->targetDEC().getSeconds());
    return "NONE#";
  }

  return "FAIL#";
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
    if (((inCmd[4] == '*') || (inCmd[4] == ':')) && (inCmd[7] == ':'))
    {
      int deg = sgn * inCmd.substring(2, 4).toInt();
      if (NORTHERN_HEMISPHERE) {
        deg = deg - 90;
      }
      else {
        deg = -90 - deg;
      }
      _mount->targetDEC().set(deg, inCmd.substring(5, 7).toInt(), inCmd.substring(8, 10).toInt());
      LOGV2(DEBUG_MEADE, F("MEADE: SetInfo: Received Target DEC: %s"), _mount->targetDEC().ToString());
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
      LOGV2(DEBUG_MEADE, F("MEADE: SetInfo: Received Target RA: %s"), _mount->targetRA().ToString());
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
      LOGV4(DEBUG_MEADE, F("MEADE: SetInfo: Received LST: %d:%d:%d"), hLST, minLST, secLST);
      _mount->setLST(lst);
    }
    else if (inCmd[1] == 'P') {
      // Set home point
      _mount->setHome(false);
      _mount->startSlewing(TRACKING);
    }
    else {
      // Set HA
      int hHA = inCmd.substring(1, 3).toInt();
      int minHA = inCmd.substring(4, 6).toInt();
      LOGV4(DEBUG_MEADE, F("MEADE: SetInfo: Received HA: %d:%d:%d"), hHA, minHA, 0);
      _mount->setHA(DayTime(hHA, minHA, 0));
    }

    return "1";
  }
  else if ((inCmd[0] == 'Y') && inCmd.length() == 19) {
    // Sync RA, DEC - current position is the given coordinate
    //   0123456789012345678
    // :SY+84*03:02.18:34:12
    int sgn = inCmd[1] == '+' ? 1 : -1;
    if (((inCmd[4] == '*') || (inCmd[4] == ':')) && (inCmd[7] == ':') && (inCmd[10] == '.') && (inCmd[13] == ':') && (inCmd[16] == ':')) {
      int deg = inCmd.substring(2, 4).toInt();
      _mount->syncPosition(inCmd.substring(11, 13).toInt(), inCmd.substring(14, 16).toInt(), inCmd.substring(17, 19).toInt(), sgn * deg + (NORTHERN_HEMISPHERE ? -90 : 90), inCmd.substring(5, 7).toInt(), inCmd.substring(8, 10).toInt());
      return "1";
    }
    return "0";
  }
  else if ((inCmd[0] == 't')) // latitude: :St+30*29#
  {
    float sgn = inCmd[1] == '+' ? 1.0f : -1.0f;
    if ((inCmd[4] == '*') || (inCmd[4] == ':')) {
      int deg = inCmd.substring(2, 4).toInt();
      int minute = inCmd.substring(5, 7).toInt();
      _mount->setLatitude(sgn * (1.0f * deg + (minute / 60.0f)));
      return "1";
    }
    return "0";
  }
  else if (inCmd[0] == 'g') // longitude :Sg097*34#
  {
    if ((inCmd[4] == '*') || (inCmd[4] == ':')) {
      int deg = inCmd.substring(1, 4).toInt();
      int minute = inCmd.substring(5, 7).toInt();
      float lon = 1.0f * deg + (1.0f * minute / 60.0f);
      if (lon > 180) {
        lon -= 360;
      }
      _mount->setLongitude(lon);
      return "1";
    }
    return "0";
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
  else if (inCmd[0] == 'A') {
    // Move Azimuth or Altitude by given arcminutes
    // :MAZ+32.1# or :MAL-32.1#
    #if AZIMUTH_ALTITUDE_MOTORS == 1
    float arcMinute = inCmd.substring(2).toFloat();
    if (inCmd[1] == 'Z'){
      _mount->moveBy(AZIMUTH_STEPS, arcMinute);
    }
    else if (inCmd[1] == 'L'){
      _mount->moveBy(ALTITUDE_STEPS, arcMinute);
    }
    #endif
    return "";
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
    _mount->goHome();
  }
  else if (inCmd[0] == 'U') {  // Unpark
    _mount->startSlewing(TRACKING);
    return "1";
  }
  return "";
}

String MeadeCommandProcessor::handleMeadeDistance(String inCmd) {
  if (_mount->isSlewingRAorDEC()){
    return "|#";
  }
  return " #";
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
    else if (inCmd[1] == 'T') {
      return String(_mount->getSpeed(TRACKING), 7) + "#";
    }
    else if (inCmd[1] == 'B') {
      return String(_mount->getBacklashCorrection()) + "#";
    }
    else if (inCmd[1] == 'M') {
      return String(_mount->getMountHardwareInfo()) + "#";
    }
    else if (inCmd[1] == 'O') {
      return getLogBuffer();
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
    else if (inCmd[1] == 'N') {
#ifdef WIFI_ENABLED
      return wifiControl.getStatus() + "#";
#endif

      return "0,#";
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
    else if (inCmd[1] == 'B') {
      _mount->setBacklashCorrection(inCmd.substring(2).toInt());
    }
  }
  else if (inCmd[0] == 'L') { // Digital Level
    #if USE_GYRO_LEVEL == 1
    if (inCmd[1] == 'G') { // get values
      if (inCmd[2] == 'R') { // get Calibration/Reference values
        return String (_mount->getPitchCalibrationAngle(),4) + "," + String (_mount->getRollCalibrationAngle(),4) +"#";
      }
      else if (inCmd[2] == 'C') { // Get current values
        auto angles = Gyro::getCurrentAngles();
        return String (angles.pitchAngle,4) + "," + String (angles.rollAngle,4) +"#";
      }
    }
    else if (inCmd[1] == 'S') { // set values
      if (inCmd[2] == 'P') { // get Calibration/Reference values
        _mount->setPitchCalibrationAngle(inCmd.substring(3).toFloat());
        return String("1#");
      }
      else if (inCmd[2] == 'R') { 
        _mount->setRollCalibrationAngle(inCmd.substring(3).toFloat());
        return String("1#");
      }
    }
    else if (inCmd[1] == '1') { // Turn on Gyro
      Gyro::startup();
      return String("1#");
    }
    else if (inCmd[1] == '0') { // Turn off Gyro
      Gyro::shutdown();
      return String("1#");
    }
    #endif
    return String("0#");
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
  switch (inCmd[0]) {
    case 'S': _mount->setSlewRate(4); break; // Slew   - Fastest
    case 'M': _mount->setSlewRate(3); break; // Find   - 2nd Fastest
    case 'C': _mount->setSlewRate(2); break; // Center - 2nd Slowest
    case 'G': _mount->setSlewRate(1); break; // Guide  - Slowest
    default:
    break;
  }
  return "";
}

String MeadeCommandProcessor::processCommand(String inCmd) {
  if (inCmd[0] == ':') {

    LOGV2(DEBUG_MEADE, F("MEADE: Received command '%s'"), inCmd.c_str());

    // Apparently some LX200 implementations put spaces in their commands..... remove them with impunity.
    int spacePos;
    while ((spacePos = inCmd.indexOf(' ')) != -1) {
      inCmd.remove(spacePos, 1);
    }

    LOGV2(DEBUG_MEADE, F("MEADE: Processing command '%s'"), inCmd.c_str());
    char command = inCmd[1];
    inCmd = inCmd.substring(2);
    switch (command) {
      case 'S': return handleMeadeSetInfo(inCmd);
      case 'M': return handleMeadeMovement(inCmd);
      case 'G': return handleMeadeGetInfo(inCmd);
      case 'g': return handleMeadeGPSCommands(inCmd);
      case 'C': return handleMeadeSyncControl(inCmd);
      case 'h': return handleMeadeHome(inCmd);
      case 'I': return handleMeadeInit(inCmd);
      case 'Q': return handleMeadeQuit(inCmd);
      case 'R': return handleMeadeSetSlewRate(inCmd);
      case 'D': return handleMeadeDistance(inCmd);
      case 'X': return handleMeadeExtraCommands(inCmd);
      default:
        LOGV2(DEBUG_MEADE, F("MEADE: Received unknown command '%s'"), inCmd.c_str());
      break;
    }
  }
  return "";
}
