#include <EEPROM.h>

#include "LcdMenu.hpp"
#include "Mount.hpp"
#include "Utility.h"

//mountstatus
#define STATUS_PARKED              0B0000000000000000
#define STATUS_SLEWING             0B0000000000000010
#define STATUS_SLEWING_TO_TARGET   0B0000000000000100
#define STATUS_SLEWING_FREE        0B0000000000000010
#define STATUS_SLEWING_MANUAL      0B0000000100000000
#define STATUS_TRACKING            0B0000000000001000
#define STATUS_PARKING             0B0000000000010000
#define STATUS_GUIDE_PULSE         0B0000000010000000
#define STATUS_GUIDE_PULSE_DIR     0B0000000001100000
#define STATUS_GUIDE_PULSE_RA      0B0000000001000000
#define STATUS_GUIDE_PULSE_DEC     0B0000000000100000
#define STATUS_GUIDE_PULSE_MASK    0B0000000011100000

// slewingStatus()
#define SLEWING_DEC                B00000010
#define SLEWING_RA                 B00000001
#define SLEWING_BOTH               B00000011
#define SLEWING_TRACKING           B00001000
#define NOT_SLEWING                B00000000

// slewStatus
#define SLEW_MASK_DEC     B0011
#define SLEW_MASK_NORTH   B0001
#define SLEW_MASK_SOUTH   B0010
#define SLEW_MASK_RA      B1100
#define SLEW_MASK_EAST    B0100
#define SLEW_MASK_WEST    B1000
#define SLEW_MASK_ANY     B1111

char* formatStringsDEC[] = {
  "",
  " %c%02d@ %02d' %02d\"",  // LCD Menu w/ cursor
  "%c%02d*%02d'%02d#",      // Meade
  "%c%02d %02d'%02d\"",     // Print
  "%c%02d@%02d'%02d\"",     // LCD display only
  "%c%02d%02d%02d",         // Compact
};

char* formatStringsRA[] = {
  "",
  " %02dh %02dm %02ds",     // LCD Menu w/ cursor
  "%02d:%02d:%02d#",        // Meade
  "%02dh %02dm %02ds",      // Print
  "%02dh%02dm%02ds",        // LCD display only
  "%02d%02d%02d",           // Compact
};

const float siderealDegreesInHour = 14.95902778;
/////////////////////////////////
//
// CTOR
//
/////////////////////////////////
Mount::Mount(int stepsPerRADegree, int stepsPerDECDegree, LcdMenu* lcdMenu) {
  _stepsPerRADegree = stepsPerRADegree;
  _stepsPerDECDegree = stepsPerDECDegree;
  _lcdMenu = lcdMenu;
  _mountStatus = 0;
  _lastDisplayUpdate = 0;
  _stepperWasRunning = false;
  _totalDECMove = 0;
  _totalRAMove = 0;
  _backlashCorrectionSteps = 16;
  _correctForBacklash = false;
  _slewingToHome = false;
  readPersistentData();
}

/////////////////////////////////
//
// readPersistentData
//
/////////////////////////////////
//
// EEPROM storage location 5 must be 0xBE for the mount to read any data
// Location 4 indicates what has been stored so far: 00000000
//                                                       ^^^^
//                                                       ||||
//                      Backlash steps (10/11) ----------+|||
//                           Speed factor (0/3) ----------+||
//     DEC stepper motor steps per degree (8/9) -----------+|
//      RA stepper motor steps per degree (6/7) ------------+
//
void Mount::readPersistentData()
{
  // Read the magic marker byte and state
  int marker = EEPROM.read(4) + EEPROM.read(5) * 256;
  if ((marker & 0xFF01) == 0xBE01) {
    _stepsPerRADegree = EEPROM.read(6) + EEPROM.read(7) * 256;
#ifdef DEBUG_MODE
    logv("EEPROM: RA Marker OK! RA steps/deg is %d", _stepsPerRADegree);
#endif
  }

  if ((marker & 0xFF02) == 0xBE02) {
    _stepsPerDECDegree = EEPROM.read(8) + EEPROM.read(9) * 256;
#ifdef DEBUG_MODE
    logv("EEPROM: DEC Marker OK! DEC steps/deg is %d", _stepsPerDECDegree);
#endif
  }

  float speed = 1.0;
  if ((marker & 0xFF04) == 0xBE04) {
    int adjust = EEPROM.read(0) + EEPROM.read(3) * 256;
    speed = 1.0 + adjust / 10000;
#ifdef DEBUG_MODE
    logv("EEPROM: Speed Marker OK! Speed adjust is %d", adjust);
#endif
  }

  if ((marker & 0xFF08) == 0xBE08) {
    _backlashCorrectionSteps = EEPROM.read(10) + EEPROM.read(11) * 256;
#ifdef DEBUG_MODE
    logv("EEPROM: Backlash Steps Marker OK! Backlash correction is %d", _backlashCorrectionSteps);
#endif
  }

  setSpeedCalibration(speed, false);
}

/////////////////////////////////
//
// writePersistentData
//
/////////////////////////////////
void Mount::writePersistentData(int which, int val)
{
  int flag = 0x00;
  int loByteLocation = 0;
  int hiByteLocation = 0;

  // If we're written something before...
  if (EEPROM.read(5) == 0xBE) {
    // ... read the current state ...
    flag = EEPROM.read(4);
  }
  switch (which) {
    case RA_STEPS:
    {
      // ... set bit 0 to indicate RA value has been written to 6/7
      flag |= 0x01;
      loByteLocation = 6;
      hiByteLocation = 7;
    }
    break;
    case DEC_STEPS:
    {
      // ... set bit 1 to indicate DEC value has been written to 8/9
      flag |= 0x02;
      loByteLocation = 8;
      hiByteLocation = 9;
    }
    break;
    case SPEED_FACTOR_DECIMALS:
    {
      // ... set bit 2 to indicate speed factor value has been written to 0/3
      flag |= 0x04;
      loByteLocation = 0;
      hiByteLocation = 3;
    }
    case BACKLASH_CORRECTION:
    {
      // ... set bit 2 to indicate speed factor value has been written to 0/3
      flag |= 0x08;
      loByteLocation = 10;
      hiByteLocation = 11;
    }
    break;
  }


  EEPROMupdate(4, flag);
  EEPROMupdate(5, 0xBE);

  EEPROMupdate(loByteLocation, val & 0x00FF);
  EEPROMupdate(hiByteLocation, (val >> 8) & 0x00FF);
}

/////////////////////////////////
//
// configureRAStepper
//
/////////////////////////////////
void Mount::configureRAStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration)
{
#ifdef NORTHERN_HEMISPHERE
  _stepperRA = new AccelStepper(stepMode, pin1, pin2, pin3, pin4);
#else
  _stepperRA = new AccelStepper(stepMode, pin4, pin3, pin2, pin1);
#endif
  _stepperRA->setMaxSpeed(maxSpeed);
  _stepperRA->setAcceleration(maxAcceleration);
  _maxRASpeed = maxSpeed;
  _maxRAAcceleration = maxAcceleration;

  // Use another AccelStepper to run the RA motor as well. This instance tracks earths rotation.
#ifdef NORTHERN_HEMISPHERE
  _stepperTRK = new AccelStepper(HALFSTEP, pin1, pin2, pin3, pin4);
#else
  _stepperTRK = new AccelStepper(HALFSTEP, pin4, pin3, pin2, pin1);
#endif
  _stepperTRK->setMaxSpeed(10);
  _stepperTRK->setAcceleration(2500);
}

/////////////////////////////////
//
// configureDECStepper
//
/////////////////////////////////
void Mount::configureDECStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration)
{
  _stepperDEC = new AccelStepper(stepMode, pin4, pin3, pin2, pin1);
  _stepperDEC->setMaxSpeed(maxSpeed);
  _stepperDEC->setAcceleration(maxAcceleration);
  _maxDECSpeed = maxSpeed;
  _maxDECAcceleration = maxAcceleration;
}

/////////////////////////////////
//
// getSpeedCalibration
//
/////////////////////////////////
float Mount::getSpeedCalibration() {
  return _trackingSpeedCalibration;
}

/////////////////////////////////
//
// setSpeedCalibration
//
/////////////////////////////////
void Mount::setSpeedCalibration(float val, bool saveToStorage) {
  _trackingSpeedCalibration = val;

  if (saveToStorage) {
    val = (val - 1.0) * 10000;
    if (val > 32766) val = 32766;
    if (val < -32766) val = -32766;
    writePersistentData(SPEED_FACTOR_DECIMALS, (int)floor(val));
  }

  // The tracker simply needs to rotate at 15degrees/hour, adjusted for sidereal
  // time (i.e. the 15degrees is per 23h56m04s. 86164s/86400 = 0.99726852. 3590/3600 is the same ratio) So we only go 15 x 0.99726852 in an hour.
  _trackingSpeed = _trackingSpeedCalibration * _stepsPerRADegree * siderealDegreesInHour / 3600.0f;
}

/////////////////////////////////
//
// getStepsPerDegree
//
/////////////////////////////////
int Mount::getStepsPerDegree(int which)
{
  if (which == RA_STEPS) {
    return _stepsPerRADegree;
  }
  if (which == DEC_STEPS) {
    return _stepsPerDECDegree;
  }
}

/////////////////////////////////
//
// setStepsPerDegree
//
/////////////////////////////////
// Function to set steps per degree for each axis. This function stores the value in persistent storage.
// The EEPROM storage location 5 is set to 0xBE if this value has ever been written. The storage location 4
// contains a bitfield indicating which values have been stored. Currently bit 0 is used for RA and bit 1 for DEC.
void Mount::setStepsPerDegree(int which, int steps) {
  if (which == DEC_STEPS) {
    writePersistentData(DEC_STEPS, steps);
    _stepsPerDECDegree = steps;

  }
  else if (which == RA_STEPS) {
    writePersistentData(RA_STEPS, steps);
    _stepsPerRADegree = steps;
  }
}

/////////////////////////////////
//
// getBacklashCorrection
//
/////////////////////////////////
int Mount::getBacklashCorrection()
{
  return _backlashCorrectionSteps;
}
/////////////////////////////////
//
// setBacklashCorrection
//
/////////////////////////////////
// Function to set steps per degree for each axis. This function stores the value in persistent storage.
// The EEPROM storage location 5 is set to 0xBE if this value has ever been written. The storage location 4
// contains a bitfield indicating which values have been stored. Currently bit 0 is used for RA and bit 1 for DEC.
void Mount::setBacklashCorrection(int steps) {
  _backlashCorrectionSteps = steps;
  writePersistentData(BACKLASH_CORRECTION, steps);
}

/////////////////////////////////
//
// setHA
//
/////////////////////////////////
void Mount::setHA(const DayTime& haTime) {
#ifdef DEBUG_MODE
  logv("setHA:  HA: %s", haTime.ToString());
#endif
  DayTime lst = DayTime(PolarisRAHour, PolarisRAMinute, PolarisRASecond);
  lst.addTime(haTime);
  setLST(lst);
  _lastHASet = millis();
}

/////////////////////////////////
//
// HA
//
/////////////////////////////////
const DayTime Mount::HA() const {
#ifdef DEBUG_MODE
  logv("Mount: Get HA.");
  logv("Mount: Polaris adjust: %s", DayTime(PolarisRAHour, PolarisRAMinute, PolarisRASecond).ToString());
#endif 
  DayTime ha = _LST;
#ifdef DEBUG_MODE
  logv("Mount: LST: %s", _LST.ToString());
#endif
  ha.subtractTime(DayTime(PolarisRAHour, PolarisRAMinute, PolarisRASecond));
#ifdef DEBUG_MODE
  logv("Mount: LST-Polaris is HA: %s", ha.ToString());
#endif
  return ha;
}

/////////////////////////////////
//
// LST
//
/////////////////////////////////
const DayTime& Mount::LST() const {
  return _LST;
}

/////////////////////////////////
//
// setLST
//
/////////////////////////////////
void Mount::setLST(const DayTime& lst) {
  _LST = lst;
  _zeroPosRA = lst;
#ifdef DEBUG_MODE
  logv("Mount: LST and ZeroPosRA set to: %s", _LST.ToString());
#endif
}

/////////////////////////////////
//
// setLatitude
//
/////////////////////////////////
void Mount::setLatitude(float lat) {
  _latitude = lat;
}
/////////////////////////////////
//
// setLongitude
//
/////////////////////////////////
void Mount::setLongitude(float lon) {
  _longitude = lon;
}

/////////////////////////////////
//
// latitude
//
/////////////////////////////////
const float Mount::latitude() const {
  return _latitude;
}
/////////////////////////////////
//
// longitude
//
/////////////////////////////////
const float Mount::longitude() const {
  return _longitude;
}

/////////////////////////////////
//
// targetRA
//
/////////////////////////////////
// Get a reference to the target RA value.
DayTime& Mount::targetRA() {
  return _targetRA;
}

/////////////////////////////////
//
// targetDEC
//
/////////////////////////////////
// Get a reference to the target DEC value.
DegreeTime& Mount::targetDEC() {
  return _targetDEC;
}

/////////////////////////////////
//
// currentRA
//
/////////////////////////////////
// Get current RA value.
const DayTime Mount::currentRA() const {
  // How many steps moves the RA ring one sidereal hour along. One sidereal hour moves just shy of 15 degrees
  float stepsPerSiderealHour = _stepsPerRADegree * siderealDegreesInHour;
  float hourPos = 2.0 * -_stepperRA->currentPosition() / stepsPerSiderealHour;
#ifdef DEBUG_MODE
  logv("CurrentRA: Steps/h    : %s (%d x %s)", String(stepsPerSiderealHour, 2).c_str(), _stepsPerRADegree, String(siderealDegreesInHour, 5).c_str());
  logv("CurrentRA: RA Steps   : %d", _stepperRA->currentPosition());
  logv("CurrentRA: POS        : %s", String(hourPos).c_str());
#endif
  hourPos += _zeroPosRA.getTotalHours();
#ifdef DEBUG_MODE
  logv("CurrentRA: ZeroPos    : %s", _zeroPosRA.ToString());
  logv("CurrentRA: POS (+zp)  : %s", DayTime(hourPos).ToString());
#endif

  bool flipRA = NORTHERN_HEMISPHERE ?
    _stepperDEC->currentPosition() < 0
    : _stepperDEC->currentPosition() > 0;
  if (flipRA)
  {
    hourPos += 12;
    if (hourPos > 24) hourPos -= 24;
#ifdef DEBUG_MODE
    logv("CurrentRA: RA (+12h): %s", DayTime(hourPos).ToString());
#endif
  }

#ifdef DEBUG_MODE
  logv("CurrentRA: RA Pos  -> : %s", DayTime(hourPos).ToString());
#endif
  return hourPos;
}

/////////////////////////////////
//
// currentDEC
//
/////////////////////////////////
// Get current DEC value.
const DegreeTime Mount::currentDEC() const {

  float degreePos = 1.0 * _stepperDEC->currentPosition() / _stepsPerDECDegree;
#ifdef DEBUG_MODE
  logv("CurrentDEC: Steps/deg  : %d", _stepsPerDECDegree);
  logv("CurrentDEC: DEC Steps  : %d", _stepperDEC->currentPosition());
  logv("CurrentDEC: POS        : %s", String(degreePos).c_str());
#endif

  if (degreePos > 0)
  {
    degreePos = -degreePos;
#ifdef DEBUG_MODE
    logv("CurrentDEC: Greater Zero, flipping.");
#endif
  }

#ifdef DEBUG_MODE
  logv("CurrentDEC: POS      : %s", DegreeTime(degreePos).ToString());
#endif
  return degreePos;
}

/////////////////////////////////
//
// syncPosition
//
/////////////////////////////////
// Set the current RA and DEC position to be the given coordinate. We do this by settign the stepper motor coordinate 
// to be at the calculated positions (that they would be if we were slewing there).
void Mount::syncPosition(int raHour, int raMinute, int raSecond, int decDegree, int decMinute, int decSecond)
{
  // Given the display RA coordinates...
  DayTime newRA = DayTime(raHour, raMinute, raSecond);
  DayTime newDEC = DayTime(decDegree, decMinute, decSecond);

  float targetRA, targetDEC;
  calculateRAandDECSteppers(targetRA, targetDEC);
  _stepperRA->setCurrentPosition(targetRA);
  _stepperDEC->setCurrentPosition(targetDEC);
}

/////////////////////////////////
//
// startSlewingToTarget
//
/////////////////////////////////
// Calculates movement parameters and program steppers to move
// there. Must call loop() frequently to actually move.
void Mount::startSlewingToTarget() {
  if (isGuiding()) {
    stopGuiding();
  }

  // Calculate new RA stepper target (and DEC)
  _currentDECStepperPosition = _stepperDEC->currentPosition();
  _currentRAStepperPosition = _stepperRA->currentPosition();
  float targetRA, targetDEC;
  calculateRAandDECSteppers(targetRA, targetDEC);
  moveSteppersTo(targetRA, targetDEC);

  _mountStatus |= STATUS_SLEWING | STATUS_SLEWING_TO_TARGET;
  _totalDECMove = 1.0f * _stepperDEC->distanceToGo();
  _totalRAMove = 1.0f * _stepperRA->distanceToGo();
}

/////////////////////////////////
//
// stopGuiding
//
/////////////////////////////////
void Mount::stopGuiding() {
  _stepperDEC->stop();
  while (_stepperDEC->isRunning()) {
    _stepperDEC->run();
  }

  _stepperDEC->setMaxSpeed(_maxDECSpeed);
  _stepperDEC->setAcceleration(_maxDECAcceleration);
  _stepperTRK->setMaxSpeed(10);
  _stepperTRK->setAcceleration(2500);
  _stepperTRK->setSpeed(_trackingSpeed);
  _mountStatus &= ~STATUS_GUIDE_PULSE_MASK;
}

/////////////////////////////////
//
// guidePulse
//
/////////////////////////////////
void Mount::guidePulse(byte direction, int duration) {
  // DEC stepper moves at sidereal rate in both directions
  // RA stepper moves at either 2x sidereal rate or stops.
  // TODO: Do we need to adjust with _trackingSpeedCalibration?
  float decTrackingSpeed = _stepsPerDECDegree * siderealDegreesInHour / 3600.0f;
  float raTrackingSpeed = _stepsPerRADegree * siderealDegreesInHour / 3600.0f;

  // TODO: Do we need to track how many steps the steppers took and add them to the GoHome calculation?
  // If so, we need to remember where we were when we started the guide pulse. Then at the end,
  // we can calculate the difference.
  // long raPos = _stepperTRK->currentPosition();
  // long decPos = _stepperDEC->currentPosition();

  switch (direction) {
    case NORTH:
    _stepperDEC->setAcceleration(2500);
    _stepperDEC->setMaxSpeed(decTrackingSpeed * 1.2);
    _stepperDEC->setSpeed(decTrackingSpeed);
    _mountStatus |= STATUS_GUIDE_PULSE | STATUS_GUIDE_PULSE_DEC;
    break;

    case SOUTH:
    _stepperDEC->setAcceleration(2500);
    _stepperDEC->setMaxSpeed(decTrackingSpeed * 1.2);
    _stepperDEC->setSpeed(-decTrackingSpeed);
    _mountStatus |= STATUS_GUIDE_PULSE | STATUS_GUIDE_PULSE_DEC;
    break;

    case WEST:
    _stepperTRK->setMaxSpeed(raTrackingSpeed * 2.2);
    _stepperTRK->setSpeed(raTrackingSpeed * 2);
    _mountStatus |= STATUS_GUIDE_PULSE | STATUS_GUIDE_PULSE_RA;
    break;

    case EAST:
    _stepperTRK->setMaxSpeed(raTrackingSpeed * 2.2);
    _stepperTRK->setSpeed(0);
    _mountStatus |= STATUS_GUIDE_PULSE | STATUS_GUIDE_PULSE_RA;
    break;
  }

  _guideEndTime = millis() + duration;
}

/////////////////////////////////
//
// runDriftAlignmentPhase
//
// Runs one of the phases of the Drift alignment
// This runs the RA motor 400 steps (about 5.3 arcminutes) in the given duration
// This function should be callsed 3 times:
// The first time with EAST, second with WEST and then with 0.
/////////////////////////////////
void Mount::runDriftAlignmentPhase(int direction, int durationSecs) {
  // Calculate the speed at which it takes the given duration to cover 400 steps.
  float speed = 400.0 / durationSecs;
  switch (direction) {
    case EAST:
    // Move 400 steps east at the calculated speed, synchronously
    _stepperRA->setAcceleration(1500);
    _stepperRA->setMaxSpeed(speed);
    _stepperRA->move(400);
    while (_stepperRA->run()) {
      yield();
    }

    // Overcome the gearing gap
    _stepperRA->setMaxSpeed(300);
    _stepperRA->move(-20);
    while (_stepperRA->run()) {
      yield();
    }
    break;

    case WEST:
    // Move 400 steps west at the calculated speed, synchronously
    _stepperRA->setMaxSpeed(speed);
    _stepperRA->move(-400);
    while (_stepperRA->run()) {
      yield();
    }
    break;

    case 0:
    // Fix the gearing to go back the other way
    _stepperRA->setMaxSpeed(300);
    _stepperRA->move(20);
    while (_stepperRA->run()) {
      yield();
    }

    // Re-configure the stepper to the correct parameters.
    _stepperRA->setAcceleration(_maxRAAcceleration);
    _stepperRA->setMaxSpeed(_maxRASpeed);
    break;
  }
}

/////////////////////////////////
//
// setManualSlewMode
//
/////////////////////////////////
void Mount::setManualSlewMode(bool state) {
  if (state) {
    stopSlewing(ALL_DIRECTIONS);
    stopSlewing(TRACKING);
    waitUntilStopped(ALL_DIRECTIONS);
    _mountStatus |= STATUS_SLEWING | STATUS_SLEWING_MANUAL;
  }
  else {
    _mountStatus &= ~STATUS_SLEWING_MANUAL;
    stopSlewing(ALL_DIRECTIONS);
    waitUntilStopped(ALL_DIRECTIONS);
    _stepperRA->setAcceleration(_maxRAAcceleration);
    _stepperRA->setMaxSpeed(_maxRASpeed);
    _stepperDEC->setMaxSpeed(_maxDECSpeed);
    _stepperDEC->setAcceleration(_maxDECAcceleration);
    startSlewing(TRACKING);
  }
}

/////////////////////////////////
//
// setSpeed
//
/////////////////////////////////
void Mount::setSpeed(int which, float speed) {
  if (which == RA_STEPS) {
    _stepperRA->setSpeed(speed);
  }
  else if (which == DEC_STEPS) {
    _stepperDEC->setSpeed(speed);
  }

}

/////////////////////////////////
//
// park
//
// Targets the mount to move to the home position and
// turns off all motors once it gets there.
/////////////////////////////////
void Mount::park() {
  stopGuiding();
  stopSlewing(ALL_DIRECTIONS | TRACKING);
  waitUntilStopped(ALL_DIRECTIONS);
  setTargetToHome();
  startSlewingToTarget();
  _mountStatus |= STATUS_PARKING;
}

/////////////////////////////////
//
// goHome
//
// Synchronously moves mount to home position 
/////////////////////////////////
void Mount::goHome()
{
  stopGuiding();
  setTargetToHome();
  startSlewingToTarget();
  _slewingToHome = true;
}

/////////////////////////////////
//
// mountStatus
//
/////////////////////////////////
byte Mount::mountStatus() {
  return _mountStatus;
}

#ifdef DEBUG_MODE
/////////////////////////////////
//
// mountStatusString
//
/////////////////////////////////
String Mount::mountStatusString() {
  if (_mountStatus == STATUS_PARKED) {
    return "PARKED";
  }
  String disp = "";
  if (_mountStatus & STATUS_PARKING) {
    disp = "PARKNG ";
  }
  else if (isGuiding()) {
    disp = "GUIDING ";
  }
  else {
    if (_mountStatus & STATUS_TRACKING) disp += "TRK ";
    if (_mountStatus & STATUS_SLEWING) disp += "SLW-";
    if (_mountStatus & STATUS_SLEWING_TO_TARGET) disp += "2TRG ";
    if (_mountStatus & STATUS_SLEWING_FREE) disp += "FR ";
    if (_mountStatus & STATUS_SLEWING_MANUAL) disp += "MAN ";

    if (_mountStatus & STATUS_SLEWING) {
      byte slew = slewStatus();
      if (slew & SLEWING_RA) disp += " SRA ";
      if (slew & SLEWING_DEC) disp += " SDEC ";
      if (slew & SLEWING_TRACKING) disp += " STRK ";
    }
  }

  disp += " RA:" + String(_stepperRA->currentPosition());
  disp += " DEC:" + String(_stepperDEC->currentPosition());
  disp += " TRK:" + String(_stepperTRK->currentPosition());

  return disp;
}
#endif

/////////////////////////////////
//
// getStatusString
//
/////////////////////////////////
String Mount::getStatusString() {
  String status;
  if (_mountStatus == STATUS_PARKED) {
    status = "Parked,";
  }
  else if (_mountStatus & STATUS_PARKING) {
    status = "Parking,";
  }
  else if (isGuiding()) {
    status = "Guiding,";
  }
  else if (slewStatus() & SLEW_MASK_ANY) {
    if (_mountStatus & STATUS_SLEWING_TO_TARGET) {
      status = "SlewToTarget,";
    }
    else if (_mountStatus & STATUS_SLEWING_FREE) {
      status = "FreeSlew,";
    }
    else if (_mountStatus & STATUS_SLEWING_MANUAL) {
      status = "ManualSlew,";
    }
    else if (slewStatus() & SLEWING_TRACKING) {
      status = "Tracking,";
    }
  }
  else {
    status = "Idle,";
  }

  String disp = "---,";
  if (_mountStatus & STATUS_SLEWING) {
    byte slew = slewStatus();
    if (slew & SLEWING_RA) disp[0] = _stepperRA->speed() < 0 ? 'R' : 'r';
    if (slew & SLEWING_DEC) disp[1] = _stepperDEC->speed() < 0 ? 'D' : 'd';
    if (slew & SLEWING_TRACKING) disp[2] = 'T';
  }
  else if (isSlewingTRK()) {
    disp[2] = 'T';
  }

  status += disp;
  status += String(_stepperRA->currentPosition()) + ",";
  status += String(_stepperDEC->currentPosition()) + ",";
  status += String(_stepperTRK->currentPosition()) + ",";

  status += RAString(COMPACT_STRING | CURRENT_STRING) + ",";
  status += DECString(COMPACT_STRING | CURRENT_STRING) + ",";

  return status;
}

/////////////////////////////////
//
// slewingStatus
//
// Returns the current state of the motors and is a bitfield with these flags:
// NOT_SLEWING is all zero. SLEWING_DEC, SLEWING_RA, SLEWING_BOTH, SLEWING_TRACKING are bits.
/////////////////////////////////
byte Mount::slewStatus() const {
  if (_mountStatus == STATUS_PARKED) {
    return NOT_SLEWING;
  }
  if (isGuiding()) {
    return NOT_SLEWING;
  }
  byte slewState = _stepperRA->isRunning() ? SLEWING_RA : NOT_SLEWING;
  slewState |= _stepperDEC->isRunning() ? SLEWING_DEC : NOT_SLEWING;

  slewState |= (_mountStatus & STATUS_TRACKING) ? SLEWING_TRACKING : NOT_SLEWING;
  return slewState;
}

/////////////////////////////////
//
// isGuiding
//
/////////////////////////////////
bool Mount::isGuiding()const
{
  return (_mountStatus & STATUS_GUIDE_PULSE);
}

/////////////////////////////////
//
// isSlewingDEC
//
/////////////////////////////////
bool Mount::isSlewingDEC() const {
  if (isParking()) return true;
  return (slewStatus() & SLEWING_DEC) != 0;
}

/////////////////////////////////
//
// isSlewingRA
//
/////////////////////////////////
bool Mount::isSlewingRA() const {
  if (isParking()) return true;
  return (slewStatus() & SLEWING_RA) != 0;
}

/////////////////////////////////
//
// isSlewingDECorRA
//
/////////////////////////////////
bool Mount::isSlewingRAorDEC() const {
  if (isParking()) return true;
  return (slewStatus() & (SLEWING_DEC | SLEWING_RA)) != 0;
}

/////////////////////////////////
//
// isSlewingIdle
//
/////////////////////////////////
bool Mount::isSlewingIdle() const {
  if (isParking()) return false;
  return (slewStatus() & (SLEWING_DEC | SLEWING_RA)) == 0;
}

/////////////////////////////////
//
// isSlewingTRK
//
/////////////////////////////////
bool Mount::isSlewingTRK() const {
  return (slewStatus() & SLEWING_TRACKING) != 0;
}

/////////////////////////////////
//
// isParked
//
/////////////////////////////////
bool Mount::isParked() const {
  return (slewStatus() == NOT_SLEWING) && (_mountStatus == STATUS_PARKED);
}

/////////////////////////////////
//
// isParking
//
/////////////////////////////////
bool Mount::isParking() const {
  return _mountStatus & STATUS_PARKING;
}

/////////////////////////////////
//
// startSlewing
//
// Starts manual slewing in one of eight directions or
// tracking, but only if not currently parking!
/////////////////////////////////
void Mount::startSlewing(int direction) {
  if (!isParking())
  {
    if (isGuiding()) {
      stopGuiding();
    }

    if (direction & TRACKING) {
      _stepperTRK->setSpeed(_trackingSpeed);

      // Turn on tracking
      _mountStatus |= STATUS_TRACKING;
    }
    else {
      int sign = NORTHERN_HEMISPHERE ? 1 : -1;
      if (direction & NORTH) {
        _stepperDEC->moveTo(sign * 30000);
        _mountStatus |= STATUS_SLEWING;
      }
      if (direction & SOUTH) {
        _stepperDEC->moveTo(-sign * 30000);
        _mountStatus |= STATUS_SLEWING;
      }
      if (direction & EAST) {
        _stepperRA->moveTo(-sign * 30000);
        _mountStatus |= STATUS_SLEWING;
      }
      if (direction & WEST) {
        _stepperRA->moveTo(sign * 30000);
        _mountStatus |= STATUS_SLEWING;
      }
    }
  }
}

/////////////////////////////////
//
// stopSlewing
//
// Stop manual slewing in one of two directions or Tracking. NS is the same. EW is the same
/////////////////////////////////
void Mount::stopSlewing(int direction) {
  if (direction & TRACKING) {
    // Turn off tracking
    _mountStatus &= ~STATUS_TRACKING;

    _stepperTRK->stop();
  }

  if ((direction & (NORTH | SOUTH)) != 0) {
    _stepperDEC->stop();
  }
  if ((direction & (WEST | EAST)) != 0) {
    _stepperRA->stop();
  }
}

/////////////////////////////////
//
// waitUntilStopped
//
/////////////////////////////////
// Block until the RA and DEC motors are stopped
void Mount::waitUntilStopped(byte direction) {
  while (((direction & (EAST | WEST)) && _stepperRA->isRunning())
         || ((direction & (NORTH | SOUTH)) && _stepperDEC->isRunning())
         || ((direction & TRACKING) && (((_mountStatus & STATUS_TRACKING) == 0) && _stepperTRK->isRunning()))
         ) {
    loop();
    yield();
  }
}

/////////////////////////////////
//
// getCurrentStepperPosition
//
/////////////////////////////////
long Mount::getCurrentStepperPosition(int direction) {
  if (direction & TRACKING) {
    return _stepperTRK->currentPosition();
  }
  if (direction & (NORTH | SOUTH)) {
    return _stepperDEC->currentPosition();
  }
  if (direction & (EAST | WEST)) {
    return _stepperRA->currentPosition();
  }
  return 0;
}

/////////////////////////////////
//
// delay
//
/////////////////////////////////
void Mount::delay(int ms) {
  long now = millis();
  while (millis() - now < ms)
  {
    loop();
    yield();
  }
}

/////////////////////////////////
//
// loop
//
// Process any stepper movement. Must be called frequently
/////////////////////////////////
void Mount::loop() {
  bool raStillRunning = false;
  bool decStillRunning = false;

  unsigned long now = millis();
#if defined DEBUG_MODE && defined SEND_PERIODIC_UPDATES
  if (now - _lastMountPrint > 2000) {
    Serial.println(getStatusString());
    _lastMountPrint = now;
  }
#endif
  if (isGuiding()) {
    if (millis() > _guideEndTime) {
      stopGuiding();
    }
    else
    {
      if (_mountStatus & STATUS_GUIDE_PULSE_RA) {
        _stepperTRK->runSpeed();
      }
      if (_mountStatus & STATUS_GUIDE_PULSE_DEC) {
        _stepperDEC->runSpeed();
      }
    }
    return;
  }

  if (_mountStatus & STATUS_TRACKING) {
    _stepperTRK->runSpeed();
  }

  if (_mountStatus & STATUS_SLEWING) {
    if (_mountStatus & STATUS_SLEWING_MANUAL) {
      _stepperDEC->runSpeed();
      _stepperRA->runSpeed();
    }
    else {
      _stepperDEC->run();
      _stepperRA->run();
    }
  }

  if (_stepperDEC->isRunning()) {
    decStillRunning = true;
  }

  if (_stepperRA->isRunning()) {
    raStillRunning = true;
  }

  if (raStillRunning || decStillRunning) {
    displayStepperPositionThrottled();
  }
  else {
    if (_mountStatus & STATUS_SLEWING_MANUAL) {
      if (_stepperWasRunning) {
        _mountStatus &= ~(STATUS_SLEWING);
      }
    }
    else {
      _mountStatus &= ~(STATUS_SLEWING | STATUS_SLEWING_TO_TARGET);

      if (_stepperWasRunning) {
#ifdef DEBUG_MODE
        logv("Loop: Reached target.");
#endif
        // Mount is at Target!
        // If we we're parking, we just reached home. Clear the flag, reset the motors and stop tracking.
        if (isParking()) {
#ifdef DEBUG_MODE
          logv("Loop:   Was Parking, stop tracking and set home.");
#endif
          stopSlewing(TRACKING);
          setHome();
        }

        _currentDECStepperPosition = _stepperDEC->currentPosition();
        _currentRAStepperPosition = _stepperRA->currentPosition();
        if (_correctForBacklash) {
#ifdef DEBUG_MODE
          logv("Loop:   Reached target at %d. Compensating by %d", _currentRAStepperPosition, _backlashCorrectionSteps);
#endif
          _currentRAStepperPosition += _backlashCorrectionSteps;
          _stepperRA->runToNewPosition(_currentRAStepperPosition);
          _correctForBacklash = false;
#ifdef DEBUG_MODE
          logv("Loop:   Backlash correction done. Pos: %d", _currentRAStepperPosition);
#endif
        }
        else
        {
#ifdef DEBUG_MODE
          logv("Loop:   Reached target at %d, no backlash compensation needed", _currentRAStepperPosition);
#endif
        }

        if (_slewingToHome) {
#ifdef DEBUG_MODE
          logv("Loop:   Was Slewing home, so setting stepper RA and TRK to zero.");
#endif
          _stepperRA->setCurrentPosition(0);
          _stepperTRK->setCurrentPosition(0);
          _targetRA = currentRA();
          if (isParking()) {
#ifdef DEBUG_MODE
            logv("Loop:   Was parking, so no tracking.");
#endif
            _mountStatus &= ~STATUS_PARKING;
          }
          else {
#ifdef DEBUG_MODE
            logv("Loop:   Restart tracking.");
#endif
            startSlewing(TRACKING);
          }
          _slewingToHome = false;
        }
        _totalDECMove = _totalRAMove = 0;

        // Make sure we do one last update when the steppers have stopped.
        displayStepperPosition();
        if (!inSerialControl) {
          _lcdMenu->updateDisplay();
        }
      }
    }
  }

  _stepperWasRunning = raStillRunning || decStillRunning;
}

/////////////////////////////////
//
// setHome
//
/////////////////////////////////
void Mount::setHome() {
#ifdef DEBUG_MODE
  logv("Mount::setHomePre: currentRA is %s", currentRA().ToString());
  logv("Mount::setHomePre: zeroPos is %s", _zeroPosRA.ToString());
  logv("Mount::setHomePre: targetRA is %s", targetRA().ToString());
#endif
  _zeroPosRA = currentRA();

  _stepperRA->setCurrentPosition(0);
  _stepperDEC->setCurrentPosition(0);
  _stepperTRK->setCurrentPosition(0);

  _targetRA = currentRA();

#ifdef DEBUG_MODE
  logv("Mount::setHomePost: currentRA is %s", currentRA().ToString());
  logv("Mount::setHomePost: zeroPos is %s", _zeroPosRA.ToString());
  logv("Mount::setHomePost: targetRA is %s", targetRA().ToString());
#endif
}

/////////////////////////////////
//
// setTargetToHome
//
// Set RA and DEC to the home position
/////////////////////////////////
void Mount::setTargetToHome() {
  float trackedSeconds = _stepperTRK->currentPosition() / _trackingSpeed; // steps/steps/s

  // In order for RA coordinates to work correctly, we need to
  // offset HATime by elapsed time since last HA set and also
  // adjust RA by the elapsed time and set it to zero.
#ifdef DEBUG_MODE
  logv("Mount::setTargetToHomePre:  currentRA is %s", currentRA().ToString());
  logv("Mount::setTargetToHomePre:  ZeroPosRA is %s", _zeroPosRA.ToString());
  logv("Mount::setTargetToHomePre:  TrackedSeconds is %f, TRK Stepper: %l", trackedSeconds, _stepperTRK->currentPosition());
  logv("Mount::setTargetToHomePre:  LST is %s", _LST.ToString());
#endif
  DayTime lst(_LST);
  lst.addSeconds(trackedSeconds);
  setLST(lst);
  _targetRA = _zeroPosRA;
  _targetRA.addSeconds(trackedSeconds);

#ifdef DEBUG_MODE
  logv("Mount::setTargetToHomePost:  currentRA is %s", currentRA().ToString());
  logv("Mount::setTargetToHomePost: ZeroPosRA is %s", _zeroPosRA.ToString());
  logv("Mount::setTargetToHomePost: LST is %s", _LST.ToString());
  logv("Mount::setTargetToHomePost: TargetRA is %s", _targetRA.ToString());
#endif

  // Set DEC to pole
  _targetDEC.set(0, 0, 0);
  _slewingToHome = true;
  // Stop the tracking stepper 
  stopSlewing(TRACKING);
}

/////////////////////////////////
//
// getSpeed
//
// Get the current speed of the stepper. NORTH, WEST, TRACKING
/////////////////////////////////
float Mount::getSpeed(int direction) {
  if (direction & TRACKING) {
    return _trackingSpeed;
  }
  if (direction & (NORTH | SOUTH)) {
    return _stepperDEC->speed();
  }
  if (direction & (EAST | WEST)) {
    return _stepperRA->speed();
  }
  return 0;
}


/////////////////////////////////
//
// calculateRAandDECSteppers
//
// This code tells the steppers to what location to move to, given the select right ascension and declination
/////////////////////////////////
void Mount::calculateRAandDECSteppers(float& targetRA, float& targetDEC) {
#ifdef DEBUG_MODE
  logv("Mount::CalcSteppersPre: Current: RA: %s, DEC: %s", currentRA().ToString(), currentDEC().ToString());
  logv("Mount::CalcSteppersPre: Target : RA: %s, DEC: %s", _targetRA.ToString(), _targetDEC.ToString());
  logv("Mount::CalcSteppersPre: ZeroRA : %s", _zeroPosRA.ToString());
  logv("Mount::CalcSteppersPre: Stepper: RA: %l, DEC: %l, TRK: %l", _stepperRA->currentPosition(), _stepperDEC->currentPosition(), _stepperTRK->currentPosition());
#endif
  DayTime raTarget = _targetRA;

  raTarget.subtractTime(_zeroPosRA);
#ifdef DEBUG_MODE
  logv("Mount::CalcSteppersIn: Adjust RA by Zeropos. New Target RA: %s, DEC: %s", raTarget.ToString(), _targetDEC.ToString());
#endif

  float hourPos = raTarget.getTotalHours();
  if (!NORTHERN_HEMISPHERE) {
    hourPos += 12;
  }
  // Map [0 to 24] range to [-12 to +12] range
  while (hourPos > 12) {
    hourPos = hourPos - 24;
#ifdef DEBUG_MODE
    logv("Mount::CalcSteppersIn: RA>12 so -24. New Target RA: %s, DEC: %s", DayTime(hourPos).ToString(), _targetDEC.ToString());
#endif
  }

  // How many steps moves the RA ring one sidereal hour along. One sidereal hour moves just shy of 15 degrees
  float stepsPerSiderealHour = _stepsPerRADegree * siderealDegreesInHour;

  // Where do we want to move RA to?
  float moveRA = hourPos * stepsPerSiderealHour / 2;

  // Where do we want to move DEC to?
  // the variable targetDEC 0deg for the celestial pole (90deg), and goes negative only.
  float moveDEC = -_targetDEC.getTotalDegrees() * _stepsPerDECDegree;

#ifdef DEBUG_MODE
  logv("Mount::CalcSteppersIn: RA Steps/deg: %d   Steps/srhour: %f", _stepsPerRADegree, stepsPerSiderealHour);
  logv("Mount::CalcSteppersIn: Target Step pos RA: %f, DEC: %f", moveRA, moveDEC);
#endif

  // We can move 6 hours in either direction. Outside of that we need to flip directions.
  float RALimit = (6.0f * stepsPerSiderealHour / 2);

  // If we reach the limit in the positive direction ...
  if (moveRA > RALimit) {
#ifdef DEBUG_MODE
    logv("Mount::CalcSteppersIn: RA is past +limit: %f, DEC: %f", RALimit);
#endif

    // ... turn both RA and DEC axis around
    float oldRA = moveRA;
    moveRA -= long(12.0f * stepsPerSiderealHour / 2);
    moveDEC = -moveDEC;
#ifdef DEBUG_MODE
    logv("Mount::CalcSteppersIn: Adjusted Target Step pos RA: %f, DEC: %f", moveRA, moveDEC);
#endif
  }
  // If we reach the limit in the negative direction...
  else if (moveRA < -RALimit) {
#ifdef DEBUG_MODE
    logv("Mount::CalcSteppersIn: RA is past -limit: %f, DEC: %f", -RALimit);
#endif
    // ... turn both RA and DEC axis around
    float oldRA = moveRA;
    moveRA += long(12.0f * stepsPerSiderealHour / 2);
    moveDEC = -moveDEC;
#ifdef DEBUG_MODE
    logv("Mount::CalcSteppersPost: Adjusted Target. Moved RA, inverted DEC");
#endif
  }

#ifdef DEBUG_MODE
  logv("Mount::CalcSteppersPost: Target Steps RA: %f, DEC: %f", moveRA, moveDEC);
#endif
  //    float targetRA = clamp(-moveRA, -RAStepperLimit, RAStepperLimit);
  //    float targetDEC = clamp(moveDEC, DECStepperUpLimit, DECStepperDownLimit);
  targetRA = -moveRA;
  targetDEC = moveDEC;

  // Can we get there without physical issues? (not doing anything with this yet)
  //  isUnreachable = ((targetRA != -moveRA) || (targetDEC != moveDEC));

  //  if (stepperRA.currentPosition() != int(targetRA)) {
  //    Serial.println("Moving RA from " + String(stepperRA.currentPosition()) + " to " + targetRA);
  //  }
  //  if (stepperDEC.currentPosition() != (targetDEC)) {
  //    Serial.println("Moving DEC from " + String(stepperDEC.currentPosition()) + " to " + targetDEC);
  //  }
}

void Mount::moveSteppersTo(float targetRA, float targetDEC) {
  // Show time: tell the steppers where to go!
  _correctForBacklash = false;
#ifdef DEBUG_MODE
  logv("Mount::MoveSteppersTo: RA  From: %l  To: %f", _stepperRA->currentPosition(), targetRA);
  logv("Mount::MoveSteppersTo: DEC From: %l  To: %f", _stepperDEC->currentPosition(), targetDEC);
#endif

  if ((_stepperRA->currentPosition() - targetRA) > 0) {
#ifdef DEBUG_MODE
    logv("Mount::MoveSteppersTo: Needs backlash correction!");
#endif
    targetRA -= _backlashCorrectionSteps;
    _correctForBacklash = true;
  }

  _stepperRA->moveTo(targetRA);
  _stepperDEC->moveTo(targetDEC);
}


/////////////////////////////////
//
// displayStepperPosition
//
/////////////////////////////////
void Mount::displayStepperPosition() {
#ifndef HEADLESS_CLIENT

  String disp;

  if ((abs(_totalDECMove) > 0.001) && (abs(_totalRAMove) > 0.001)) {
    float decDist = 100.0 - 100.0 * _stepperDEC->distanceToGo() / _totalDECMove;
    float raDist = 100.0 - 100.0 * _stepperRA->distanceToGo() / _totalRAMove;

    sprintf(scratchBuffer, "R %s %d%%", RAString(LCD_STRING | CURRENT_STRING).c_str(), (int)raDist);
    _lcdMenu->setCursor(0, 0);
    _lcdMenu->printMenu(String(scratchBuffer));
    sprintf(scratchBuffer, "D%s %d%%", DECString(LCD_STRING | CURRENT_STRING).c_str(), (int)decDist);
    _lcdMenu->setCursor(0, 1);
    _lcdMenu->printMenu(String(scratchBuffer));
    return;
  }
  else if (abs(_totalDECMove) > 0.001) {
    float decDist = 100.0 - 100.0 * _stepperDEC->distanceToGo() / _totalDECMove;
    sprintf(scratchBuffer, "D%s %d%%", DECString(LCD_STRING | CURRENT_STRING).c_str(), (int)decDist);
    _lcdMenu->setCursor(0, 1);
    _lcdMenu->printMenu(String(scratchBuffer));
  }
  else if (abs(_totalRAMove) > 0.001) {
    float raDist = 100.0 - 100.0 * _stepperRA->distanceToGo() / _totalRAMove;
    sprintf(scratchBuffer, "R %s %d%%", RAString(LCD_STRING | CURRENT_STRING).c_str(), (int)raDist);
    disp = disp + String(scratchBuffer);
    _lcdMenu->setCursor(0, inSerialControl ? 0 : 1);
    _lcdMenu->printMenu(String(scratchBuffer));
  }
  else {
#ifdef SUPPORT_SERIAL_CONTROL
    if (inSerialControl) {
      sprintf(scratchBuffer, " RA: %s", RAString(LCD_STRING | CURRENT_STRING).c_str());
      _lcdMenu->setCursor(0, 0);
      _lcdMenu->printMenu(scratchBuffer);
      sprintf(scratchBuffer, "DEC: %s", DECString(LCD_STRING | CURRENT_STRING).c_str());
      _lcdMenu->setCursor(0, 1);
      _lcdMenu->printMenu(scratchBuffer);
    }
    else {
      sprintf(scratchBuffer, "R%s D%s", RAString(COMPACT_STRING | CURRENT_STRING).c_str(), DECString(COMPACT_STRING | CURRENT_STRING).c_str());
      _lcdMenu->setCursor(0, 1);
      _lcdMenu->printMenu(scratchBuffer);
    }
#else
    sprintf(scratchBuffer, "R%s D%s", RAString(COMPACT_STRING | CURRENT_STRING).c_str(), DECString(COMPACT_STRING | CURRENT_STRING).c_str());
    _lcdMenu->setCursor(0, 1);
    _lcdMenu->printMenu(scratchBuffer);
#endif
  }
#endif
}

/////////////////////////////////
//
// displayStepperPositionThrottled
//
/////////////////////////////////
void Mount::displayStepperPositionThrottled() {
#ifndef HEADLESS_CLIENT
  long elapsed = millis() - _lastDisplayUpdate;
  if (elapsed > DISPLAY_UPDATE_TIME) {
    displayStepperPosition();
    _lastDisplayUpdate = millis();
  }
#endif
}

/////////////////////////////////
//
// DECString
//
// Return a string of DEC in the given format. For LCDSTRING, active determines where the cursor is
/////////////////////////////////
String Mount::DECString(byte type, byte active) {
  DegreeTime dec;
  if ((type & TARGET_STRING) == TARGET_STRING) {
#ifdef DEBUG_MODE
    logv("DECString: TARGET!");
#endif
    dec = _targetDEC;
  }
  else {
#ifdef DEBUG_MODE
    logv("DECString: CURRENT!");
#endif
    dec = DegreeTime(currentDEC());
  }
#ifdef DEBUG_MODE
  logv("DECString: Precheck  : %s", dec.ToString());
#endif
  dec.checkHours();
#ifdef DEBUG_MODE
  logv("DECString: Postcheck : %s", dec.ToString());
#endif

  sprintf(scratchBuffer, formatStringsDEC[type & FORMAT_STRING_MASK], dec.getPrintDegrees() > 0 ? '+' : '-', int(fabs(dec.getPrintDegrees())), dec.getMinutes(), dec.getSeconds());
  if ((type & FORMAT_STRING_MASK) == LCDMENU_STRING) {
    scratchBuffer[active * 4 + (active > 0 ? 1 : 0)] = '>';
  }

  return String(scratchBuffer);
}

/////////////////////////////////
//
// RAString
//
/////////////////////////////////
// Return a string of RA in the given format. For LCDSTRING, active determines where the cursor is
String Mount::RAString(byte type, byte active) {
  DayTime ra;
  if ((type & TARGET_STRING) == TARGET_STRING) {
    ra = DayTime(_targetRA);
  }
  else {
    ra = DayTime(currentRA());
  }

  sprintf(scratchBuffer, formatStringsRA[type & FORMAT_STRING_MASK], ra.getHours(), ra.getMinutes(), ra.getSeconds());
  if ((type & FORMAT_STRING_MASK) == LCDMENU_STRING) {
    scratchBuffer[active * 4] = '>';
  }
  return String(scratchBuffer);
}
