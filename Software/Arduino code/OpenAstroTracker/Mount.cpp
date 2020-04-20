#include "LcdMenu.hpp"

#include "Mount.hpp"

//mountstatus
#define STATUS_PARKED              B00000000
#define STATUS_SLEWING             B00000010
#define STATUS_SLEWING_TO_TARGET   B00000100
#define STATUS_SLEWING_FREE        B00000010
#define STATUS_TRACKING            B00001000
#define STATUS_PARKING             B00010000
#define STATUS_GUIDE_PULSE         B10000000
#define STATUS_GUIDE_PULSE_DIR     B01100000
#define STATUS_GUIDE_PULSE_RA      B01000000
#define STATUS_GUIDE_PULSE_DEC     B00100000
#define STATUS_GUIDE_PULSE_MASK    B11100000

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
};

char* formatStringsRA[] = {
  "",
  " %02dh %02dm %02ds",     // LCD Menu w/ cursor
  "%02d:%02d:%02d#",        // Meade
  "%02dh %02dm %02ds",      // Print
  "%02dh%02dm%02ds",        // LCD display only
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
  setSpeedCalibration(1.0);
}

/////////////////////////////////
//
// configureRAStepper
//
/////////////////////////////////
void Mount::configureRAStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration)
{
  _stepperRA = new AccelStepper(stepMode, pin1, pin2, pin3, pin4);
  _stepperRA->setMaxSpeed(maxSpeed);
  _stepperRA->setAcceleration(maxAcceleration);
  _maxRASpeed = maxSpeed;
  _maxRAAcceleration = maxAcceleration;

  // Use another AccelStepper to run the RA motor as well. This instance tracks earths rotation.
  _stepperTRK = new AccelStepper(HALFSTEP, pin1, pin2, pin3, pin4);
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

float Mount::getSpeedCalibration() {
  return _trackingSpeedCalibration;
}

void Mount::setSpeedCalibration(float val) {
  _trackingSpeedCalibration = val;

  // The tracker simply needs to rotate at 15degrees/hour, adjusted for sidereal
  // time (i.e. the 15degrees is per 23h56m04s. 86164s/86400 = 0.99726852. 3590/3600 is the same ratio) So we only go 15 x 0.99726852 in an hour.
  _trackingSpeed = _trackingSpeedCalibration * _stepsPerRADegree * siderealDegreesInHour / 3600.0f;
}

/////////////////////////////////
//
// setHA
//
/////////////////////////////////
void Mount::setHA(const DayTime & haTime) {
  _HATime = haTime;
  _HACorrection.set(_HATime);
  _HACorrection.subtractTime(_HAAdjust);
  _lastHASet = millis();
}

/////////////////////////////////
//
// setHACorrection
//
/////////////////////////////////
void Mount::setHACorrection(int h, int m, int s) {
  _HAAdjust.set(h, m, s);
}

/////////////////////////////////
//
// getHACorrection
//
/////////////////////////////////
DayTime Mount::getHACorrection() {
  return _HAAdjust;
}
/////////////////////////////////
//
// HA
//
/////////////////////////////////
const DayTime& Mount::HA() const {
  return _HATime;
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
  if (!isSlewingRA() ||  (_mountStatus & STATUS_SLEWING_TO_TARGET) == 0) return _currentRA;

  // How many steps are needed between current and target
  long deltaSteps = _stepperRA->targetPosition() - _currentRAStepperPosition;

  // Calculate how far along (0..1) we are
  float alongPathNormalized = 1.0 * (_stepperRA->currentPosition() - _currentRAStepperPosition) / deltaSteps;

  float raT =  _targetRA.getTotalHours();
  float raC =  _currentRA.getTotalHours();
  float deltaT = raT - raC;

  // If we're rolling over, take the short route. i.e. if the motor direction indicates
  // the other direction than the math, move the target beyond current in the right direction
  if ( (raT < raC && (deltaSteps < 0)) || (raT > raC && (deltaSteps > 0))) {
    raT += (deltaSteps < 0) ? 24 : -24;
    deltaT = raT - raC;
  }

  // Move the RA along by wher we are, fractionally
  raC += deltaT * alongPathNormalized;

  return raC;
}

/////////////////////////////////
//
// currentDEC
//
/////////////////////////////////
// Get current DEC value.
const DegreeTime Mount::currentDEC() const {
  if (!isSlewingDEC() ||  (_mountStatus & STATUS_SLEWING_TO_TARGET) == 0) return _currentDEC;

  // How many steps are needed between current and target
  long deltaSteps = _stepperDEC->targetPosition() - _currentDECStepperPosition;

  // Calculate how far along (0..1) we are
  float alongPathNormalized = 1.0 * (_stepperDEC->currentPosition() - _currentDECStepperPosition) / deltaSteps;

  float decT =  _targetDEC.getTotalDegrees();
  float decC =  _currentDEC.getTotalDegrees();
  float deltaT = decT - decC;
  decC += deltaT * alongPathNormalized;
  return decC;
}

/////////////////////////////////
//
// syncRA
//
/////////////////////////////////
// Set the current RA position to be the given time. We do this by adjusting HA by the
// difference between the current RA and the RA that we were told we were actually at.
void Mount::syncRA(int hour, int minute, int second) {
  // Given the display RA coordinates...
  DayTime newRA = DayTime(hour, minute, second);

  // ... convert to the system RA values
  newRA.subtractTime(_HACorrection);

  // Calculate the difference between the new system RA and the current system RA
  DayTime deltaRA = DayTime(newRA);
  deltaRA.subtractTime(_currentRA);

  // Now add this difference to HA.
  DayTime newHA = DayTime(_HATime);
  newHA.addTime(deltaRA);
  setHA(newHA);

  float targetRA, targetDEC;
  calculateRAandDECSteppers(targetRA, targetDEC);
  _stepperRA->setCurrentPosition(targetRA);
}

/////////////////////////////////
//
// syncDEC
//
/////////////////////////////////
// Set the current DEC position to be the given degrees (which are 0 .. -180 for Northern Hemisphere)
void Mount::syncDEC(int degree, int minute, int second) {
  _currentDEC = DegreeTime(degree, minute, second);
  _targetDEC = _currentDEC;
  float targetRA, targetDEC;
  calculateRAandDECSteppers(targetRA, targetDEC);
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
      _mountStatus |= STATUS_GUIDE_PULSE | STATUS_GUIDE_PULSE_DEC ;
      break;

    case SOUTH:
      _stepperDEC->setAcceleration(2500);
      _stepperDEC->setMaxSpeed(decTrackingSpeed * 1.2);
      _stepperDEC->setSpeed(-decTrackingSpeed);
      _mountStatus |= STATUS_GUIDE_PULSE | STATUS_GUIDE_PULSE_DEC ;
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
      _stepperRA->runToPosition();

      // Overcome the gearing gap
      _stepperRA->setMaxSpeed(300);
      _stepperRA->move(-20);
      _stepperRA->runToPosition();
      break;

    case WEST:
      // Move 400 steps west at the calculated speed, synchronously
      _stepperRA->setMaxSpeed(speed);
      _stepperRA->move(-400);
      _stepperRA->runToPosition();
      break;

    case 0 :
      // Fix the gearing to go back the other way
      _stepperRA->setMaxSpeed(300);
      _stepperRA->move(20);
      _stepperRA->runToPosition();

      // Re-configure the stepper to the correct parameters.
      _stepperRA->setAcceleration(_maxRAAcceleration);
      _stepperRA->setMaxSpeed(_maxRASpeed);
      break;
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
// Synchronously moves mount to home position and
// sets Tracking mode according to argument
/////////////////////////////////
void Mount::goHome(bool tracking)
{
  stopGuiding();
  stopSlewing(TRACKING);
  setTargetToHome();
  startSlewingToTarget();
  waitUntilStopped(ALL_DIRECTIONS);
  setHome();
  if (tracking) {
    startSlewing(TRACKING);
  }
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
    if (_mountStatus & STATUS_SLEWING) disp += "SLW ";
    if (_mountStatus & STATUS_SLEWING_TO_TARGET) disp += "2TRG ";
    if (_mountStatus & STATUS_SLEWING_FREE) disp += "FR ";

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
// slewingStatus
//
// Returns the current state of the motors and is a bitfield with these flags:
// NOT_SLEWING is all zero. SLEWING_DEC, SLEWING_RA, SLEWING_BOTH, SLEWING_TRACKING are bits.
/////////////////////////////////
byte Mount::slewStatus() {
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
bool Mount::isGuiding()
{
  return (_mountStatus & STATUS_GUIDE_PULSE);
}

/////////////////////////////////
//
// isSlewingDEC
//
/////////////////////////////////
bool Mount::isSlewingDEC() {
  if (isParking()) return true;
  return (slewStatus() & SLEWING_DEC) != 0;
}

/////////////////////////////////
//
// isSlewingRA
//
/////////////////////////////////
bool Mount::isSlewingRA() {
  if (isParking()) return true;
  return (slewStatus() & SLEWING_RA) != 0;
}

/////////////////////////////////
//
// isSlewingDECorRA
//
/////////////////////////////////
bool Mount::isSlewingRAorDEC() {
  if (isParking()) return true;
  return (slewStatus() & (SLEWING_DEC | SLEWING_RA)) != 0;
}

/////////////////////////////////
//
// isSlewingIdle
//
/////////////////////////////////
bool Mount::isSlewingIdle() {
  if (isParking()) return false;
  return (slewStatus() & (SLEWING_DEC | SLEWING_RA)) == 0;
}

/////////////////////////////////
//
// isSlewingTRK
//
/////////////////////////////////
bool Mount::isSlewingTRK() {
  return (slewStatus() & SLEWING_TRACKING) != 0;
}

/////////////////////////////////
//
// isParked
//
/////////////////////////////////
bool Mount::isParked() {
  return (slewStatus() == NOT_SLEWING) && (_mountStatus == STATUS_PARKED);
}

/////////////////////////////////
//
// isParking
//
/////////////////////////////////
bool Mount::isParking() {
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
      if (direction & NORTH) {
        _stepperDEC->moveTo(30000);
        _mountStatus |= STATUS_SLEWING;
      }
      if (direction & SOUTH ) {
        _stepperDEC->moveTo(-30000);
        _mountStatus |= STATUS_SLEWING;
      }
      if (direction & EAST ) {
        _stepperRA->moveTo(-30000);
        _mountStatus |= STATUS_SLEWING;
      }
      if (direction & WEST) {
        _stepperRA->moveTo(30000);
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

  if ((direction & (NORTH | SOUTH)) != 0)  {
    _stepperDEC->stop();
  }
  if ((direction & (WEST | EAST)) != 0)  {
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
  while ( ((direction & (EAST | WEST)) && _stepperRA->isRunning())
          || ((direction & (NORTH | SOUTH)) && _stepperDEC->isRunning())
          || ((direction & TRACKING) && (((_mountStatus & STATUS_TRACKING) == 0) && _stepperTRK->isRunning()))
        ) {
    loop();
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
#ifdef DEBUG_MODE
  if (now - _lastMountPrint > 1500) {
    Serial.println(mountStatusString());
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
    _stepperDEC->run();
    _stepperRA->run();
  }

  if (_stepperDEC->isRunning()) {
    decStillRunning = true;
  }

  if (_stepperRA->isRunning()) {
    raStillRunning = true;
  }

  if (raStillRunning || decStillRunning) {
    if (!raStillRunning) {
      _currentRA = _targetRA;
    }
    if (!decStillRunning) {
      _currentDEC = _targetDEC;
    }

    displayStepperPositionThrottled();
  }
  else {
    _mountStatus &= ~(STATUS_SLEWING | STATUS_SLEWING_TO_TARGET);

    if (_stepperWasRunning) {
      // Mount is at Target!
      _currentRA = _targetRA;
      _currentDEC = _targetDEC;

      // If we we're parking, we just reached home. Clear the flag, reset the motors and stop tracking.
      if (isParking()) {
        _mountStatus &= ~STATUS_PARKING;
        stopSlewing(TRACKING);
        setHome();
      }

      _currentDECStepperPosition = _stepperDEC->currentPosition();
      _currentRAStepperPosition = _stepperRA->currentPosition();
      _totalDECMove = _totalRAMove = 0;

      // Make sure we do one last update when the steppers have stopped.
      displayStepperPosition();
      if (!inSerialControl) {
        _lcdMenu->updateDisplay();
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
  _stepperRA->setCurrentPosition(0);
  _stepperDEC->setCurrentPosition(0);
  _stepperTRK->setCurrentPosition(0);
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
  _targetRA.set(0, 0, 0);
  _targetRA.addSeconds(trackedSeconds);
  DayTime ha(_HATime);
  ha.addSeconds(trackedSeconds);
  setHA(ha);

  // Set DEC to pole
  _targetDEC.set(0, 0, 0);
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
void Mount::calculateRAandDECSteppers(float& targetRA, float &targetDEC) {
  float hourPos = _targetRA.getTotalHours();
  // Map [0 to 24] range to [-12 to +12] range
  if (hourPos > 12) {
    hourPos = hourPos - 24;
  }

  // How many steps moves the RA ring one sidereal hour along. One sidereal hour moves just shy of 15 degrees
  float stepsPerSiderealHour = _stepsPerRADegree * siderealDegreesInHour;

  // Where do we want to move RA to?
  float moveRA = hourPos * stepsPerSiderealHour  / 2;

  // Where do we want to move DEC to?
  // the variable targetDEC 0deg for the celestial pole (90deg), and goes negative only.
  float moveDEC = -_targetDEC.getTotalDegrees() * _stepsPerDECDegree;

  // We can move 6 hours in either direction. Outside of that we need to flip directions.
  float RALimit = (6.0f * stepsPerSiderealHour / 2);

  // If we reach the limit in the positive direction ...
  if (moveRA > RALimit) {
    // ... turn both RA and DEC axis around
    float oldRA = moveRA;
    moveRA -= long(12.0f * stepsPerSiderealHour  / 2);
    moveDEC = -moveDEC;
  }
  // If we reach the limit in the negative direction...
  else if (moveRA < -RALimit) {
    // ... turn both RA and DEC axis around
    float oldRA = moveRA;
    moveRA += long(12.0f * stepsPerSiderealHour  / 2);
    moveDEC = -moveDEC;
  }

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

  String disp ;

  if ((abs(_totalDECMove) > 0.001) && (abs(_totalRAMove) > 0.001)) {
    float decDist = 100.0 - 100.0 * _stepperDEC->distanceToGo() / _totalDECMove;
    float raDist = 100.0 - 100.0 * _stepperRA->distanceToGo() / _totalRAMove;

    sprintf(scratchBuffer, "R %s %d%%", RAString(LCD_STRING | CURRENT_STRING).c_str(), (int)raDist);
    _lcdMenu->setCursor(0, 0);
    _lcdMenu->printMenu(String(scratchBuffer));
    sprintf(scratchBuffer, "D %s %d%%", DECString(LCD_STRING | CURRENT_STRING).c_str(), (int)decDist);
    _lcdMenu->setCursor(0, 1);
    _lcdMenu->printMenu(String(scratchBuffer));
    return;
  }
  else if (abs(_totalDECMove) > 0.001) {
    float decDist = 100.0 - 100.0 * _stepperDEC->distanceToGo() / _totalDECMove;
    sprintf(scratchBuffer, "D %s %d%%", DECString(LCD_STRING | CURRENT_STRING).c_str(), (int)decDist);
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
      sprintf(scratchBuffer, " RA:  %s", RAString(LCD_STRING | CURRENT_STRING).c_str());
      _lcdMenu->setCursor(0, 0);
      _lcdMenu->printMenu(scratchBuffer);
      sprintf(scratchBuffer, "DEC: %s", DECString(LCD_STRING | CURRENT_STRING).c_str());
      _lcdMenu->setCursor(0, 1);
      _lcdMenu->printMenu(scratchBuffer);
    }
    else {
      disp = "R:" + String(_stepperRA->currentPosition());
      _lcdMenu->setCursor(0, 1);
      _lcdMenu->printMenu(disp);
      disp = "D:" + String(_stepperDEC->currentPosition()) ;
      _lcdMenu->setCursor(8, 1);
      _lcdMenu->printMenu(disp);
    }
#else
    disp = "R:" + String(_stepperRA->currentPosition());
    _lcdMenu->setCursor(0, 1);
    _lcdMenu->printMenu(disp);
    disp = "D:" + String(_stepperDEC->currentPosition()) ;
    _lcdMenu->setCursor(8, 1);
    _lcdMenu->printMenu(disp);
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
    dec = DegreeTime(_targetDEC);
  } else {
    dec = DegreeTime (currentDEC());
  }
  dec.checkHours();

  sprintf(scratchBuffer, formatStringsDEC[type & FORMAT_STRING_MASK], dec.getPrintDegrees() > 0 ? '+' : '-', int(fabs(dec.getPrintDegrees() )), dec.getMinutes() , dec.getSeconds());
  if ((type & FORMAT_STRING_MASK) == LCDMENU_STRING) {
    scratchBuffer[active * 4 + (active > 0 ? 1 : 0)] = '>';
  }

  return String (scratchBuffer);
}

/////////////////////////////////
//
// RAString
//
/////////////////////////////////
// Return a string of DEC in the given format. For LCDSTRING, active determines where the cursor is
String Mount::RAString(byte type, byte active) {
  DayTime ra;
  if ((type & TARGET_STRING) == TARGET_STRING) {
    ra = DayTime(_targetRA);
  } else {
    ra = DayTime(currentRA());
  }

  DayTime raDisplay(ra);
  raDisplay.addTime(_HACorrection);

  sprintf(scratchBuffer, formatStringsRA[type & FORMAT_STRING_MASK], raDisplay.getHours(), raDisplay.getMinutes() , raDisplay.getSeconds());
  if ((type & FORMAT_STRING_MASK) == LCDMENU_STRING) {
    scratchBuffer[active * 4] = '>';
  }
  return String (scratchBuffer);
}
