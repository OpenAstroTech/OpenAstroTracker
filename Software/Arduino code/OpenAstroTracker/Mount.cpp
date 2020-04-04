#include "LcdMenu.hpp"
#include "Mount.hpp"

//mountstatus
#define STATUS_PARKED              B00000000
#define STATUS_SLEWING             B00000010
#define STATUS_SLEWING_TO_TARGET   B00000100
#define STATUS_SLEWING_FREE        B00000010
#define STATUS_TRACKING            B00001000

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
  " %02d@ %02d' %02d\"",    // LCD Menu w/ cursor
  "%02d:%02d:%02d#",        // Meade
  "%02dh %02dm %02ds",      // Print
  "%02d@%02d'%02d\"",       // LCD display only
};

/////////////////////////////////
//
// CTOR
//
/////////////////////////////////
Mount::Mount(int stepsPerRAHour, int stepsPerDECDegree, LcdMenu* lcdMenu) {
  _stepsPerRAHour = stepsPerRAHour;
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
  _stepperDEC = new AccelStepper(stepMode, pin1, pin2, pin3, pin4);
  _stepperDEC->setMaxSpeed(maxSpeed);
  _stepperDEC->setAcceleration(maxAcceleration);
}

float Mount::getSpeedCalibration() {
  return _trackingSpeedCalibration;
}

void Mount::setSpeedCalibration(float val) {
  _trackingSpeedCalibration = val;

  // The tracker simply needs to rotate at 15degrees/hour, adjusted for sidereal
  // time (i.e. the 15degrees is per 23h56m04s. 86164s/86400 = 0.99726852. 3590/3600 is the same ratio).
  _trackingSpeed = _trackingSpeedCalibration * _stepsPerRAHour * 14.95903 / 3600.0f;
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
  float targetRA = _targetRA.getTotalHours();
  // Serial.print("TargetRA: " + String(targetRA, 4));
  if (!_stepperRA->isRunning()) {
    return targetRA;
  }
  float degreesToGo = 1.0 * _stepperRA->distanceToGo() / _stepsPerRAHour;
  // Serial.print("  Deg2Go: " + String(degreesToGo , 4));
  float hoursToGo = degreesToGo / 15.0;
  // Serial.print("  Hrs2Go: " + String(hoursToGo , 4));
  float currentHour = targetRA - hoursToGo;
  // Serial.println("  CurHrs: " + String(currentHour, 4));
  return currentHour;
}

/////////////////////////////////
//
// currentDEC
//
/////////////////////////////////
// Get current DEC value.
const DegreeTime Mount::currentDEC() const {
  float targetDEC = _targetDEC.getTotalDegrees();
  // Serial.print("TargetDEC: " + String(targetDEC, 4));
  if (!_stepperDEC->isRunning()) {
    return targetDEC;
  }
  float degreesToGo = 1.0 * _stepperDEC->distanceToGo() / _stepsPerDECDegree;
  // Serial.print("  Deg2Go: " + String(degreesToGo , 4));
  float currentDegree = targetDEC - degreesToGo;
  // Serial.println("  CurDeg: " + String(currentDegree, 4));
  return currentDegree;
}

/////////////////////////////////
//
// startSlewingToTarget
//
/////////////////////////////////
// Calculates movement parameters and program steppers to move
// there. Must call loop() frequently to actually move.
void Mount::startSlewingToTarget() {
  //Serial.println("StSlew2Trgt!");
  // Calculate new RA stepper target (and DEC)
  calculateRAandDECSteppers();
  _mountStatus |= STATUS_SLEWING | STATUS_SLEWING_TO_TARGET;
  _totalDECMove = 1.0f * abs(_stepperDEC->distanceToGo());
  _totalRAMove = 1.0f * abs(_stepperRA->distanceToGo());
  // Serial.println("StSlew2Trgt: M " + String(_mountStatus) + " Totals: R" + String(_totalRAMove) + "D" + String(_totalDECMove));
}

/////////////////////////////////
//
// mountStatus
//
/////////////////////////////////
byte Mount::mountStatus() {
  return _mountStatus;
}

#ifdef DEBUG
/////////////////////////////////
//
// mountStatusString
//
/////////////////////////////////
String Mount::mountStatusString() {
  if (_mountStatus == STATUS_PARKED) {
    return "PRKD";
  }
  String disp = "";
  if (_mountStatus & STATUS_TRACKING) disp += "TRK ";
  if (_mountStatus & STATUS_SLEWING) disp += "SLW ";
  if (_mountStatus & STATUS_SLEWING_TO_TARGET) disp += "2TRG ";
  if (_mountStatus & STATUS_SLEWING_FREE) disp += "FR ";

  if (_mountStatus & STATUS_SLEWING) {
    byte slew = slewingStatus();
    if (slew & SLEWING_RA) disp += " SRA ";
    if (slew & SLEWING_DEC) disp += " SDEC ";
    if (slew & SLEWING_TRACKING) disp += " STRK ";
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
  byte slewState = _stepperRA->isRunning() ? SLEWING_RA : NOT_SLEWING;
  slewState |= _stepperDEC->isRunning() ? SLEWING_DEC : NOT_SLEWING;

  slewState |= (_mountStatus & STATUS_TRACKING) ? SLEWING_TRACKING : NOT_SLEWING;
  return slewState;
}

/////////////////////////////////
//
// isSlewingDEC
//
/////////////////////////////////
bool Mount::isSlewingDEC() {
  return (slewStatus() & SLEWING_DEC) != 0;
}

/////////////////////////////////
//
// isSlewingRA
//
/////////////////////////////////
bool Mount::isSlewingRA() {
  return (slewStatus() & SLEWING_RA) != 0;
}

/////////////////////////////////
//
// isSlewingDECorRA
//
/////////////////////////////////
bool Mount::isSlewingRAorDEC() {
  return (slewStatus() & (SLEWING_DEC | SLEWING_RA)) != 0;
}

/////////////////////////////////
//
// isSlewingIdle
//
/////////////////////////////////
bool Mount::isSlewingIdle() {
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
  return slewStatus() == NOT_SLEWING;
}

/////////////////////////////////
//
// startSlewing
//
// Starts manual slewing in one of eight directions or tracking
/////////////////////////////////
void Mount::startSlewing(int direction) {
  // Serial.println("StrtSlew! " + String(direction));
  if (direction & TRACKING) {
    _stepperTRK->setSpeed(_trackingSpeed);

    // Turn on tracking
    _mountStatus |= STATUS_TRACKING;
  }
  else {
    if (direction & NORTH) {
      _stepperDEC->moveTo(-30000);
      _mountStatus |= STATUS_SLEWING;
    }
    if (direction & SOUTH ) {
      _stepperDEC->moveTo(30000);
      _mountStatus |= STATUS_SLEWING;
    }
    if (direction & EAST ) {
      _stepperRA->moveTo(30000);
      _mountStatus |= STATUS_SLEWING;
    }
    if (direction & WEST) {
      _stepperRA->moveTo(-30000);
      _mountStatus |= STATUS_SLEWING;
    }
  }
  // Serial.println("StrtSlew: M " + String(_mountStatus));
}

/////////////////////////////////
//
// stopSlewing
//
// Stop manual slewing in one of two directions or Tracking. NS is the same. EW is the same
/////////////////////////////////
void Mount::stopSlewing(int direction) {
  // Serial.println("StpSlw: " + String(direction));
  if (direction & TRACKING) {
    // Turn off tracking
    _mountStatus &= ~STATUS_TRACKING;
    _stepperTRK->stop();
    // Serial.println("StpSlw: trk");
  }

  if ((direction & (NORTH | SOUTH)) != 0)  {
    _stepperDEC->stop();
    // Serial.println("StpSlw: DEC " + String(_mountStatus));
  }
  if ((direction & (WEST | EAST)) != 0)  {
    _stepperRA->stop();
    // Serial.println("StpSlw: RA " + String(_mountStatus));
  }

  //  // Serial.println("StopSlewing: Wait4Stop " + String(_mountStatus));
  //  while (_stepperRA->isRunning() || _stepperDEC->isRunning() || (((_mountStatus & STATUS_TRACKING)==0) && _stepperTRK->isRunning())) {
  //    // Serial.print("StopSlewing: ");
  //    loop();
  //  }

  // Serial.println("StpSlw: ok " + String(_mountStatus));
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
    // Serial.print("W4Stp: ");
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
// loop
//
// Process any stepper movement. Must be called frequently
/////////////////////////////////
void Mount::loop() {
  bool raStillRunning = false;
  bool decStillRunning = false;

  unsigned long now = millis();
  if (now - _lastMountPrint > 25) {
    // Serial.println(mountStatusString());
    _lastMountPrint = now;
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
    //    _stepperDEC->run();
  }

  if (_stepperRA->isRunning()) {
    raStillRunning = true;
    //  _stepperRA->run();
  }

  if (raStillRunning || decStillRunning) {
    //Serial.println("Loop: RA " + String(raStillRunning) + String(decStillRunning) );
    displayStepperPositionThrottled();
  }
  else {
    _mountStatus &= ~(STATUS_SLEWING | STATUS_SLEWING_TO_TARGET);

    if (_stepperWasRunning) {
      // Serial.println("LP: MV DONE" );
      // Make sure we do one last update when the steppers have stopped.
      displayStepperPosition();
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
  // Serial.println("Pos: " + String(stepperTRK->currentPosition()) + " Secs:" + String(trackedSeconds , 3));

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
    return _stepperTRK->speed();
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
void Mount::calculateRAandDECSteppers() {
  float hourPos = _targetRA.getTotalHours();
  // Map [0 to 24] range to [-12 to +12] range
  if (hourPos > 12) {
    hourPos = hourPos - 24;
  }

  // How many steps moves the RA ring one sidereal hour along. One sidereal hour moves just shy of 15 degrees
  float stepsPerSiderealHour = _stepsPerRAHour * 14.95903;

  // Where do we want to move RA to?
  float moveRA = hourPos * stepsPerSiderealHour  / 2;

  // Where do we want to move DEC to?
  // the variable targetDEC 0deg for the celestial pole (90deg), and goes negative only.
  float moveDEC = _targetDEC.getTotalDegrees() * _stepsPerDECDegree;

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
  float targetRA = -moveRA;
  float targetDEC = moveDEC;

  // Can we get there without physical issues? (not doing anything with this yet)
  //  isUnreachable = ((targetRA != -moveRA) || (targetDEC != moveDEC));

  //  if (stepperRA.currentPosition() != int(targetRA)) {
  //    Serial.println("Moving RA from " + String(stepperRA.currentPosition()) + " to " + targetRA);
  //  }
  //  if (stepperDEC.currentPosition() != (targetDEC)) {
  //    Serial.println("Moving DEC from " + String(stepperDEC.currentPosition()) + " to " + targetDEC);
  //  }

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
  _lcdMenu->setCursor(0, 1);
  String disp ;
  if (_totalDECMove > 0) {
    float decDist = 100.0 - 100.0 * abs(_stepperDEC->distanceToGo()) / _totalDECMove;
    // float dec = currentDEC();
    // DayTime dt(dec);
    // int degree = dt.getHours() + (north ? 90 : -90);
    // Serial.println("DEC: " + String(dec, 4) + " DT: " + String(degree) + " " + String(dt.getMinutes()) + " " + String(dt.getSeconds()));
    // sprintf(scratchBuffer, "D: %02d@%02d'%02d\" %d%%", degree, dt.getMinutes(), dt.getSeconds(), (int)decDist);
    sprintf(scratchBuffer, "D: %d%%", (int)decDist);
    disp = String(scratchBuffer);
  }
  else {
    disp = "D:" + String(_stepperDEC->currentPosition());
  }
  if (_totalRAMove > 0) {
    float raDist = 100.0 - 100.0 * abs(_stepperRA->distanceToGo()) / _totalRAMove;
    // float ra = currentRA();
    // DayTime dt(ra);
    // dt.addTime(HACorrection);
    // Serial.println("RA: " + String(ra, 4) + " DT: " + dt.ToString());
    // sprintf(scratchBuffer, " R: %02d@%02d'%02d\" %d%%", dt.getHours(), dt.getMinutes(), dt.getSeconds(), (int)raDist);
    sprintf(scratchBuffer, " R: %d%%", (int)raDist);
    disp = disp + String(scratchBuffer);
  }
  else {
    disp = disp + " R:" + String(_stepperRA->currentPosition());
  }

  _lcdMenu->printMenu(disp);
}

/////////////////////////////////
//
// displayStepperPositionThrottled
//
/////////////////////////////////
void Mount::displayStepperPositionThrottled() {
  long elapsed = millis() - _lastDisplayUpdate;
  if (elapsed > DISPLAY_UPDATE_TIME) {
    displayStepperPosition();
    _lastDisplayUpdate = millis();
  }
}

/////////////////////////////////
//
// DECString
//
// Return a string of DEC in the given format. For LCDSTRING, active determines where the cursor is
/////////////////////////////////
String Mount::DECString(byte type, byte active) {
  DegreeTime dec(currentDEC());
  if ((type & TARGET_STRING) == TARGET_STRING) {
    dec = DegreeTime(_targetDEC);
  }

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
  //Serial.println("RA1: T" + String(type) + " A" + String(active));
  DayTime ra(currentRA());
  //Serial.println("RA2:" + ra.ToString());
  if ((type & TARGET_STRING) == TARGET_STRING) {
    //Serial.println("RA3");
    ra = DayTime(_targetRA);
    //Serial.println("RA3A:" + ra.ToString());
  }

  //Serial.println("RA4:" + ra.ToString());
  DayTime raDisplay(ra);
  //Serial.println("RA5:" + raDisplay.ToString());

  if (type & (LCDMENU_STRING | LCD_STRING | PRINT_STRING) != 0) {
    //Serial.println("RA6:" + _HACorrection.ToString());
    raDisplay.addTime(_HACorrection);
    //Serial.println("RA6A:" + raDisplay.ToString());
  }

  //Serial.println("RA7:" + raDisplay.ToString());

  sprintf(scratchBuffer, formatStringsRA[type & FORMAT_STRING_MASK], raDisplay.getHours(), raDisplay.getMinutes() , raDisplay.getSeconds());
  //Serial.println("RA8:" + String(scratchBuffer));
  if ((type & FORMAT_STRING_MASK) == LCDMENU_STRING) {
    //Serial.println("RA9");
    scratchBuffer[active * 4] = '>';
    //Serial.println("RA9A:" + String(scratchBuffer));
  }
  //Serial.println("RAQ");
  return String (scratchBuffer);
}
