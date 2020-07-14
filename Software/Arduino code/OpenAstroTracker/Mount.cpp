#include "InterruptCallback.hpp"

#include "LcdMenu.hpp"
#include "Mount.hpp"
#include "Utility.hpp"
#include "EPROMStore.hpp"

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
#define STATUS_FINDING_HOME        0B0010000000000000

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

const char* formatStringsDEC[] = {
  "",
  " %c%02d@ %02d' %02d\"",  // LCD Menu w/ cursor
  "%c%02d*%02d'%02d#",      // Meade
  "%c%02d %02d'%02d\"",     // Print
  "%c%02d@%02d'%02d\"",     // LCD display only
  "%c%02d%02d%02d",         // Compact
};

const char* formatStringsRA[] = {
  "",
  " %02dh %02dm %02ds",     // LCD Menu w/ cursor
  "%02d:%02d:%02d#",        // Meade
  "%02dh %02dm %02ds",      // Print
  "%02dh%02dm%02ds",        // LCD display only
  "%02d%02d%02d",           // Compact
};

/////////////////////////////////
// This is the callback function for the timer interrupt. It does very minimal work,
// only stepping the stepper motors as needed.
/////////////////////////////////
void mountLoop(void* payload) {
  Mount* mount = reinterpret_cast<Mount*>(payload);
  mount->interruptLoop();
}

const float siderealDegreesInHour = 14.95902778;
/////////////////////////////////
//
// CTOR
//
/////////////////////////////////
Mount::Mount(int stepsPerRADegree, int stepsPerDECDegree, LcdMenu* lcdMenu) {
  #if RA_Driver_TYPE != 0
  _stepsPerRADegree = stepsPerRADegree * SET_MICROSTEPPING; // hier
  #else
  _stepsPerRADegree = stepsPerRADegree;
  #endif
  #if DEC_Driver_TYPE != 0
  _stepsPerDECDegree = stepsPerDECDegree * DEC_MICROSTEPPING;
  #else
  _stepsPerDECDegree = stepsPerDECDegree;
  #endif
  _lcdMenu = lcdMenu;
  _mountStatus = 0;
  _lastDisplayUpdate = 0;
  _stepperWasRunning = false;
  _totalDECMove = 0;
  _totalRAMove = 0;
  _moveRate = 4;
#if RA_Stepper_TYPE == 0
  _backlashCorrectionSteps = 16;
#else
  _backlashCorrectionSteps = 0;
#endif
  _correctForBacklash = false;
  _slewingToHome = false;
  readPersistentData();
}

/////////////////////////////////
//
// startTimerInterrupts
//
/////////////////////////////////
void Mount::startTimerInterrupts()
{
#ifndef ESPBOARD
  // 2 kHz updates
  if (!InterruptCallback::setInterval(1.0f, mountLoop, this))
  {
    LOGV1(DEBUG_MOUNT, "Mount:: CANNOT setup interrupt timer!");
  }
#endif // !ESPBOARD

}

/////////////////////////////////
//
// readConfiguration
//
/////////////////////////////////
void Mount::readConfiguration()
{
  LOGV1(DEBUG_INFO, "Mount: Reading configuration data from EEPROM");
  readPersistentData();
  LOGV1(DEBUG_INFO, "Mount: Done reading configuration data from EEPROM");
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
  uint8_t markerLo=EPROMStore::Storage()->read(4);
  uint8_t markerHi=EPROMStore::Storage()->read(5);
  
  uint16_t marker = (uint16_t)markerLo + (uint16_t)markerHi * 256;
  LOGV4(DEBUG_INFO, "Mount: EEPROM: Marker: %x (L:%d  H:%d)", marker, markerLo, markerHi);

  if ((marker & 0xFF01) == 0xBE01) {
    _stepsPerRADegree = EPROMStore::Storage()->read(6) + EPROMStore::Storage()->read(7) * 256;
    LOGV2(DEBUG_INFO,"Mount: EEPROM: RA Marker OK! RA steps/deg is %d", _stepsPerRADegree);
  }
  else{
    LOGV1(DEBUG_INFO,"Mount: EEPROM: No stored value for RA steps");
  }

  if ((marker & 0xFF02) == 0xBE02) {
    _stepsPerDECDegree = EPROMStore::Storage()->read(8) + EPROMStore::Storage()->read(9) * 256;
    LOGV2(DEBUG_INFO,"Mount: EEPROM: DEC Marker OK! DEC steps/deg is %d", _stepsPerDECDegree);
  }
  else{
    LOGV1(DEBUG_INFO,"Mount: EEPROM: No stored value for DEC steps");
  }

  float speed = 1.0;
  if ((marker & 0xFF04) == 0xBE04) {
    int adjust = EPROMStore::Storage()->read(0) + EPROMStore::Storage()->read(3) * 256;
    speed = 1.0 + 1.0 * adjust / 10000.0;
    LOGV3(DEBUG_INFO,"Mount: EEPROM: Speed Marker OK! Speed adjust is %d, speedFactor is %f", adjust, speed);
  }
  else{
    LOGV1(DEBUG_INFO,"Mount: EEPROM: No stored value for speed factor");
  }

  if ((marker & 0xFF08) == 0xBE08) {
    _backlashCorrectionSteps = EPROMStore::Storage()->read(10) + EPROMStore::Storage()->read(11) * 256;
    LOGV2(DEBUG_INFO,"Mount: EEPROM: Backlash Steps Marker OK! Backlash correction is %d", _backlashCorrectionSteps);
  }
    else{
    LOGV1(DEBUG_INFO,"Mount: EEPROM: No stored value for backlash correction");
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
  uint8_t flag = 0x00;
  int loByteLocation = 0;
  int hiByteLocation = 0;

  // If we're written something before...
  uint8_t magicMarker = EPROMStore::Storage()->read(5);
  LOGV4(DEBUG_INFO,"Mount: EEPROM Write: Marker is %x, flag is %x (%d)", magicMarker, flag, flag);
  if (magicMarker == 0xBE) {
    // ... read the current state ...
    flag = EPROMStore::Storage()->read(4);
    LOGV3(DEBUG_INFO,"Mount: EEPROM Write: Marker is 0xBE, flag is %x (%d)", flag, flag);
  }
  switch (which) {
    case RA_STEPS:
    {
      // ... set bit 0 to indicate RA value has been written to 6/7
      flag |= 0x01;
      loByteLocation = 6;
      hiByteLocation = 7;
      LOGV2(DEBUG_INFO,"Mount: EEPROM Write: Updating RA steps to %d", val);
    }
    break;
    case DEC_STEPS:
    {
      // ... set bit 1 to indicate DEC value has been written to 8/9
      flag |= 0x02;
      loByteLocation = 8;
      hiByteLocation = 9;
      LOGV2(DEBUG_INFO,"Mount: EEPROM Write: Updating DEC steps to %d", val);
    }
    break;
    case SPEED_FACTOR_DECIMALS:
    {
      // ... set bit 2 to indicate speed factor value has been written to 0/3
      flag |= 0x04;
      loByteLocation = 0;
      hiByteLocation = 3;
      LOGV2(DEBUG_INFO,"Mount: EEPROM Write: Updating Speed factor to %d", val);
    }
    break;
    case BACKLASH_CORRECTION:
    {
      // ... set bit 2 to indicate speed factor value has been written to 0/3
      flag |= 0x08;
      loByteLocation = 10;
      hiByteLocation = 11;
      LOGV2(DEBUG_INFO,"Mount: EEPROM Write: Updating Backlash to %d", val);
    }
    break;
  }

  LOGV3(DEBUG_INFO,"Mount: EEPROM Write: New Marker is 0xBE, flag is %x (%d)", flag, flag);

  EPROMStore::Storage()->update(4, flag);
  EPROMStore::Storage()->update(5, 0xBE);

  EPROMStore::Storage()->update(loByteLocation, val & 0x00FF);
  EPROMStore::Storage()->update(hiByteLocation, (val >> 8) & 0x00FF);

  LOGV5(DEBUG_INFO,"Mount: EEPROM Write: Wrote %x to %d and %x to %d", val & 0x00FF, loByteLocation, (val >> 8) & 0x00FF, hiByteLocation);
}

/////////////////////////////////
//
// configureRAStepper
//
/////////////////////////////////
#if RA_Stepper_TYPE == 0    // 28BYJ
void Mount::configureRAStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration)
{
#if NORTHERN_HEMISPHERE
  _stepperRA = new AccelStepper(stepMode, pin4, pin3, pin2, pin1);
#else
  _stepperRA = new AccelStepper(stepMode, pin1, pin2, pin3, pin4);
#endif
  _stepperRA->setMaxSpeed(maxSpeed);
  _stepperRA->setAcceleration(maxAcceleration);
  _maxRASpeed = maxSpeed;
  _maxRAAcceleration = maxAcceleration;

  // Use another AccelStepper to run the RA motor as well. This instance tracks earths rotation.
#if NORTHERN_HEMISPHERE
  _stepperTRK = new AccelStepper(HALFSTEP, pin4, pin3, pin2, pin1);
#else
  _stepperTRK = new AccelStepper(HALFSTEP, pin1, pin2, pin3, pin4);
#endif
  _stepperTRK->setMaxSpeed(10);
  _stepperTRK->setAcceleration(2500);
}
#endif

#if RA_Stepper_TYPE == 1    //NEMA
void Mount::configureRAStepper(byte stepMode, byte pin1, byte pin2, int maxSpeed, int maxAcceleration)
{
  _stepperRA = new AccelStepper(stepMode, pin1, pin2);
  _stepperRA->setMaxSpeed(maxSpeed);
  _stepperRA->setAcceleration(maxAcceleration);
  _maxRASpeed = maxSpeed;
  _maxRAAcceleration = maxAcceleration;

  // Use another AccelStepper to run the RA motor as well. This instance tracks earths rotation.
  _stepperTRK = new AccelStepper(DRIVER, pin1, pin2);
  _stepperTRK->setMaxSpeed(10);
  _stepperTRK->setAcceleration(2500);
}
#endif

/////////////////////////////////
//
// configureDECStepper
//
/////////////////////////////////
#if DEC_Stepper_TYPE == 0   // 28BYJ
void Mount::configureDECStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration)
{
#if NORTHERN_HEMISPHERE
  _stepperDEC = new AccelStepper(stepMode, pin1, pin2, pin3, pin4);
#else
  _stepperDEC = new AccelStepper(stepMode, pin4, pin3, pin2, pin1);
#endif
  _stepperDEC->setMaxSpeed(maxSpeed);
  _stepperDEC->setAcceleration(maxAcceleration);
  _maxDECSpeed = maxSpeed;
  _maxDECAcceleration = maxAcceleration;
}
#endif

#if DEC_Stepper_TYPE == 1   // NEMA
void Mount::configureDECStepper(byte stepMode, byte pin1, byte pin2, int maxSpeed, int maxAcceleration)
{
  _stepperDEC = new AccelStepper(stepMode, pin1, pin2);
  _stepperDEC->setMaxSpeed(maxSpeed);
  _stepperDEC->setAcceleration(maxAcceleration);
  _maxDECSpeed = maxSpeed;
  _maxDECAcceleration = maxAcceleration;
}
#endif

/////////////////////////////////
//
// configureRAdriver
// TMC2209 UART only
/////////////////////////////////
#if RA_Driver_TYPE == 3
void Mount::configureRAdriver(HardwareSerial *serial, float rsense, byte driveraddress, int rmscurrent, int stallvalue)
{
  _driverRA = new TMC2209Stepper(serial, rsense, driveraddress);
  _driverRA->begin();
  //_driverRA->en_spreadCycle(1);
  _driverRA->blank_time(24);
  _driverRA->rms_current(rmscurrent);
  _driverRA->microsteps(16);
  _driverRA->TCOOLTHRS(0xFFFFF);
  _driverRA->semin(5);
  _driverRA->semax(2);
  _driverRA->sedn(0b01);
  _driverRA->SGTHRS(10);
}
#endif

/////////////////////////////////
//
// configureDECdriver
// TMC2209 UART only
/////////////////////////////////
#if DEC_Driver_TYPE == 3
void Mount::configureDECdriver(HardwareSerial *serial, float rsense, byte driveraddress, int rmscurrent, int stallvalue)
{
  _driverDEC = new TMC2209Stepper(serial, rsense, driveraddress);
  _driverDEC->begin();
  _driverDEC->blank_time(24);
  //_driverDEC->en_spreadCycle(1);
  _driverDEC->rms_current(rmscurrent);
  _driverDEC->microsteps(DEC_MICROSTEPPING);
  _driverDEC->TCOOLTHRS(0xFFFFF);
  _driverDEC->semin(5);
  _driverDEC->semax(2);
  _driverDEC->sedn(0b01);
  _driverDEC->SGTHRS(stallvalue);
  _driverDEC->ihold(DEC_HOLDCURRENT);
}
#endif

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
  LOGV3(DEBUG_MOUNT, "Mount: Updating speed calibration from %f to %f", _trackingSpeedCalibration , val);
  _trackingSpeedCalibration = val;

  LOGV2(DEBUG_MOUNT, "Mount: Current tracking speed is %f steps/sec", _trackingSpeed);

  // The tracker simply needs to rotate at 15degrees/hour, adjusted for sidereal
  // time (i.e. the 15degrees is per 23h56m04s. 86164s/86400 = 0.99726852. 3590/3600 is the same ratio) So we only go 15 x 0.99726852 in an hour.
  #if RA_Driver_TYPE == 3 //hier
  _trackingSpeed = _trackingSpeedCalibration * ((_stepsPerRADegree / SET_MICROSTEPPING) * TRACKING_MICROSTEPPING) * siderealDegreesInHour / 3600.0f;
  #else
  _trackingSpeed = _trackingSpeedCalibration * _stepsPerRADegree * siderealDegreesInHour / 3600.0f;
  #endif
  LOGV2(DEBUG_MOUNT, "Mount: New tracking speed is %f steps/sec", _trackingSpeed);

  if (saveToStorage) {
    val = (val - 1.0) * 10000;
    if (val > 32766) val = 32766;
    if (val < -32766) val = -32766;
    writePersistentData(SPEED_FACTOR_DECIMALS, (int)floor(val));
  }

  // If we are currently tracking, update the speed.
  if (isSlewingTRK()) {
    _stepperTRK->setSpeed(_trackingSpeed);
  }
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

  return 0;
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
void Mount::setSlewRate(int rate) 
{
  _moveRate = clamp(rate, 1, 4);
  float speedFactor[] = { 0, 0.05, 0.2, 0.5, 1.0};
  LOGV3(DEBUG_MOUNT,"Mount::setSlewRate: rate is %d -> %f",_moveRate , speedFactor[_moveRate ]);
  _stepperDEC->setMaxSpeed(speedFactor[_moveRate ] * _maxDECSpeed);
  _stepperRA->setMaxSpeed(speedFactor[_moveRate ] * _maxRASpeed);
  LOGV3(DEBUG_MOUNT,"Mount::setSlewRate: new speeds are RA: %f  DEC: %f",_stepperRA->maxSpeed(), _stepperDEC->maxSpeed());
}

/////////////////////////////////
//
// setHA
//
/////////////////////////////////
void Mount::setHA(const DayTime& haTime) {
  LOGV2(DEBUG_MOUNT,"Mount: setHA:  HA is %s", haTime.ToString());
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
  LOGV1(DEBUG_MOUNT_VERBOSE,"Mount: Get HA.");
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount: Polaris adjust: %s", DayTime(PolarisRAHour, PolarisRAMinute, PolarisRASecond).ToString());
  DayTime ha = _LST;
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount: LST: %s", _LST.ToString());
  ha.subtractTime(DayTime(PolarisRAHour, PolarisRAMinute, PolarisRASecond));
  LOGV2(DEBUG_MOUNT,"Mount: GetHA: LST-Polaris is HA %s", ha.ToString());
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
  LOGV2(DEBUG_MOUNT,"Mount: Set LST and ZeroPosRA to: %s", _LST.ToString());
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
  #if RA_Driver_TYPE == 0
  float hourPos = 2.0 * -_stepperRA->currentPosition() / stepsPerSiderealHour;
  #else
  float hourPos =  -_stepperRA->currentPosition() / stepsPerSiderealHour;
  #endif
  LOGV4(DEBUG_MOUNT_VERBOSE,"CurrentRA: Steps/h    : %s (%d x %s)", String(stepsPerSiderealHour, 2).c_str(), _stepsPerRADegree, String(siderealDegreesInHour, 5).c_str());
  LOGV2(DEBUG_MOUNT_VERBOSE,"CurrentRA: RA Steps   : %d", _stepperRA->currentPosition());
  LOGV2(DEBUG_MOUNT_VERBOSE,"CurrentRA: POS        : %s", String(hourPos).c_str());
  hourPos += _zeroPosRA.getTotalHours();
  LOGV2(DEBUG_MOUNT_VERBOSE,"CurrentRA: ZeroPos    : %s", _zeroPosRA.ToString());
  LOGV2(DEBUG_MOUNT_VERBOSE,"CurrentRA: POS (+zp)  : %s", DayTime(hourPos).ToString());

  bool flipRA = NORTHERN_HEMISPHERE ?
    _stepperDEC->currentPosition() < 0
    : _stepperDEC->currentPosition() > 0;
  if (flipRA)
  {
    hourPos += 12;
    if (hourPos > 24) hourPos -= 24;
    LOGV2(DEBUG_MOUNT_VERBOSE,"CurrentRA: RA (+12h): %s", DayTime(hourPos).ToString());
  }

  LOGV2(DEBUG_MOUNT_VERBOSE,"CurrentRA: RA Pos  -> : %s", DayTime(hourPos).ToString());
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
  LOGV2(DEBUG_MOUNT_VERBOSE,"CurrentDEC: Steps/deg  : %d", _stepsPerDECDegree);
  LOGV2(DEBUG_MOUNT_VERBOSE,"CurrentDEC: DEC Steps  : %d", _stepperDEC->currentPosition());
  LOGV2(DEBUG_MOUNT_VERBOSE,"CurrentDEC: POS        : %s", String(degreePos).c_str());

  if (degreePos > 0)
  {
    degreePos = -degreePos;
    LOGV1(DEBUG_MOUNT_VERBOSE,"CurrentDEC: Greater Zero, flipping.");
  }

  LOGV2(DEBUG_MOUNT_VERBOSE,"CurrentDEC: POS      : %s", DegreeTime(degreePos).ToString());
  return degreePos;
}

/////////////////////////////////
//
// syncPosition
//
/////////////////////////////////
// Set the current RA and DEC position to be the given coordinate. We do this by setting the stepper motor coordinate 
// to be at the calculated positions (that they would be if we were slewing there).
void Mount::syncPosition(int raHour, int raMinute, int raSecond, int decDegree, int decMinute, int decSecond)
{
  _targetRA.set(raHour,raMinute,raSecond);
  _targetDEC.set(decDegree,decMinute,decSecond);

  float targetRA, targetDEC;
  LOGV7(DEBUG_MOUNT, "Mount: Sync Position to RA: %d:%d:%d and DEC: %d*%d:%d", raHour, raMinute, raSecond, decDegree, decMinute, decSecond);
  calculateRAandDECSteppers(targetRA, targetDEC);
  LOGV3(DEBUG_MOUNT, "Mount: Sync Stepper Position is RA: %d and DEC: %d", targetRA, targetDEC);
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

  // set Slew microsteps for TMC2209 UART // hier
  #if RA_Driver_TYPE == 3
  _driverRA->microsteps(SET_MICROSTEPPING);
  //_driverRA->en_spreadCycle(1);  //only used as audiofeedback for quick debug
  #endif

  // Make sure we're slewing at full speed on a GoTo
  _stepperDEC->setMaxSpeed(_maxDECSpeed);
  _stepperRA->setMaxSpeed(_maxRASpeed);

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

#if DEBUG_LEVEL&(DEBUG_MOUNT|DEBUG_MOUNT_VERBOSE)
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
// isFindingHome
//
/////////////////////////////////
bool Mount::isFindingHome() const {
  return _mountStatus & STATUS_FINDING_HOME;
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

      // Set move rate to last commanded slew rate
      setSlewRate(_moveRate);
      #if RA_Driver_TYPE == 3
      _driverRA->microsteps(SET_MICROSTEPPING);
      //_driverRA->en_spreadCycle(1);  //only used as audiofeedback for quick debug
      //hier
      #endif
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
  unsigned long now = millis();
  while (millis() - now < (unsigned long)ms)
  {
    loop();
    yield();
  }
}

/////////////////////////////////
//
// interruptLoop()
//
// This function is run in an ISR. It needs to be fast and do little work.
/////////////////////////////////
void Mount::interruptLoop()
{
  if (_mountStatus & STATUS_GUIDE_PULSE) {
    if (_mountStatus & STATUS_GUIDE_PULSE_RA) {
      _stepperTRK->runSpeed();
    }
    if (_mountStatus & STATUS_GUIDE_PULSE_DEC) {
      _stepperDEC->runSpeed();
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
}

/////////////////////////////////
//
// loop
//
// Process any stepper changes. 
/////////////////////////////////
void Mount::loop() {
  bool raStillRunning = false;
  bool decStillRunning = false;

  // Since some of the boards cannot process timer interrupts at the required 
  // speed (or at all), we'll just stick to deterministic calls here.
  #if RUN_STEPPERS_IN_MAIN_LOOP == 1
  interruptLoop();
  #endif

  #if DEBUG_LEVEL&DEBUG_MOUNT 
  unsigned long now = millis();
  if (now - _lastMountPrint > 2000) {
    Serial.println(getStatusString());
    _lastMountPrint = now;
  }
  #endif
  
  #if RA_Driver_TYPE == 3 && DEC_Driver_TYPE == 3 && USE_AUTOHOME == 1
  if (isFindingHome()) {
    if (digitalRead(DEC_DIAG_PIN) == HIGH) {
      finishFindingHomeDEC();
      return;
    }
    if (digitalRead(RA_DIAG_PIN) == 52) {
      finishFindingHomeRA();
      return;
    }    
    //return;
  }
  #endif
  
  if (isGuiding()) {
    if (millis() > _guideEndTime) {
      stopGuiding();
    }
    return;
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
        LOGV1(DEBUG_MOUNT,"Mount::Loop: Reached target.");
        // Mount is at Target!
        // If we we're parking, we just reached home. Clear the flag, reset the motors and stop tracking.
        if (isParking()) {
          LOGV1(DEBUG_MOUNT,"Mount::Loop:   Was Parking, stop tracking and set home.");
          stopSlewing(TRACKING);
          setHome();
        }

        _currentDECStepperPosition = _stepperDEC->currentPosition();
        _currentRAStepperPosition = _stepperRA->currentPosition();
        #if RA_Driver_TYPE == 3
        _driverRA->microsteps(TRACKING_MICROSTEPPING);
        //_driverRA->en_spreadCycle(0); // only for audio feedback for quick debug
        #endif
        if (_correctForBacklash) {
          LOGV3(DEBUG_MOUNT,"Mount::Loop:   Reached target at %d. Compensating by %d", _currentRAStepperPosition, _backlashCorrectionSteps);
          _currentRAStepperPosition += _backlashCorrectionSteps;
          _stepperRA->runToNewPosition(_currentRAStepperPosition);
          _correctForBacklash = false;
          LOGV2(DEBUG_MOUNT,"Mount::Loop:   Backlash correction done. Pos: %d", _currentRAStepperPosition);
        }
        else
        {
          LOGV2(DEBUG_MOUNT,"Mount::Loop:   Reached target at %d, no backlash compensation needed", _currentRAStepperPosition);
        }

        if (_slewingToHome) {
          LOGV1(DEBUG_MOUNT,"Mount::Loop:   Was Slewing home, so setting stepper RA and TRK to zero.");
          _stepperRA->setCurrentPosition(0);
          _stepperTRK->setCurrentPosition(0);
          _targetRA = currentRA();
          if (isParking()) {
            LOGV1(DEBUG_MOUNT,"Mount::Loop:   Was parking, so no tracking.");
            _mountStatus &= ~STATUS_PARKING;
          }
          else {
            LOGV1(DEBUG_MOUNT,"Mount::Loop:   Restart tracking.");
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
  LOGV1(DEBUG_MOUNT,"Mount::setHome() called");
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setHomePre: currentRA is %s", currentRA().ToString());
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setHomePre: zeroPos is %s", _zeroPosRA.ToString());
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setHomePre: targetRA is %s", targetRA().ToString());
  _zeroPosRA = currentRA();

  _stepperRA->setCurrentPosition(0);
  _stepperDEC->setCurrentPosition(0);
  _stepperTRK->setCurrentPosition(0);

  _targetRA = currentRA();

  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setHomePost: currentRA is %s", currentRA().ToString());
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setHomePost: zeroPos is %s", _zeroPosRA.ToString());
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setHomePost: targetRA is %s", targetRA().ToString());
}

/////////////////////////////////
//
// setTargetToHome
//
// Set RA and DEC to the home position
/////////////////////////////////
void Mount::setTargetToHome() {
  float trackedSeconds = _stepperTRK->currentPosition() / _trackingSpeed; // steps/steps/s

  LOGV2(DEBUG_MOUNT,"Mount::setTargetToHome() called with %fs elapsed tracking", trackedSeconds);

  // In order for RA coordinates to work correctly, we need to
  // offset HATime by elapsed time since last HA set and also
  // adjust RA by the elapsed time and set it to zero.
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setTargetToHomePre:  currentRA is %s", currentRA().ToString());
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setTargetToHomePre:  ZeroPosRA is %s", _zeroPosRA.ToString());
  LOGV3(DEBUG_MOUNT_VERBOSE,"Mount::setTargetToHomePre:  TrackedSeconds is %f, TRK Stepper: %l", trackedSeconds, _stepperTRK->currentPosition());
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setTargetToHomePre:  LST is %s", _LST.ToString());
  DayTime lst(_LST);
  lst.addSeconds(trackedSeconds);
  setLST(lst);
  _targetRA = _zeroPosRA;
  _targetRA.addSeconds(trackedSeconds);

  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setTargetToHomePost:  currentRA is %s", currentRA().ToString());
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setTargetToHomePost: ZeroPosRA is %s", _zeroPosRA.ToString());
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setTargetToHomePost: LST is %s", _LST.ToString());
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::setTargetToHomePost: TargetRA is %s", _targetRA.ToString());

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
  LOGV3(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersPre: Current: RA: %s, DEC: %s", currentRA().ToString(), currentDEC().ToString());
  LOGV3(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersPre: Target : RA: %s, DEC: %s", _targetRA.ToString(), _targetDEC.ToString());
  LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersPre: ZeroRA : %s", _zeroPosRA.ToString());
  LOGV4(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersPre: Stepper: RA: %l, DEC: %l, TRK: %l", _stepperRA->currentPosition(), _stepperDEC->currentPosition(), _stepperTRK->currentPosition());
  DayTime raTarget = _targetRA;

  raTarget.subtractTime(_zeroPosRA);
  LOGV3(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersIn: Adjust RA by Zeropos. New Target RA: %s, DEC: %s", raTarget.ToString(), _targetDEC.ToString());

  float hourPos = raTarget.getTotalHours();
  if (!NORTHERN_HEMISPHERE) {
    hourPos += 12;
  }
  // Map [0 to 24] range to [-12 to +12] range
  while (hourPos > 12) {
    hourPos = hourPos - 24;
    LOGV3(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersIn: RA>12 so -24. New Target RA: %s, DEC: %s", DayTime(hourPos).ToString(), _targetDEC.ToString());
  }

  // How many steps moves the RA ring one sidereal hour along. One sidereal hour moves just shy of 15 degrees
  float stepsPerSiderealHour = _stepsPerRADegree * siderealDegreesInHour;

  // Where do we want to move RA to?
  #if RA_Driver_TYPE == 0
  float moveRA = hourPos * stepsPerSiderealHour / 2;
  #else
  float moveRA = hourPos * stepsPerSiderealHour;
  #endif



  // Where do we want to move DEC to?
  // the variable targetDEC 0deg for the celestial pole (90deg), and goes negative only.
  float moveDEC = -_targetDEC.getTotalDegrees() * _stepsPerDECDegree;

  LOGV3(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersIn: RA Steps/deg: %d   Steps/srhour: %f", _stepsPerRADegree, stepsPerSiderealHour);
  LOGV3(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersIn: Target Step pos RA: %f, DEC: %f", moveRA, moveDEC);

  // We can move 6 hours in either direction. Outside of that we need to flip directions.
#if RA_Stepper_TYPE == 0
  float RALimit = (6.0f * stepsPerSiderealHour / 2);
#else
  float RALimit = (6.0f * stepsPerSiderealHour);
#endif

  // If we reach the limit in the positive direction ...
  if (moveRA > RALimit) {
    LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersIn: RA is past +limit: %f, DEC: %f", RALimit);

    // ... turn both RA and DEC axis around
#if RA_Stepper_TYPE == 0
    moveRA -= long(12.0f * stepsPerSiderealHour / 2);
#else
    moveRA -= long(12.0f * stepsPerSiderealHour);
#endif
    moveDEC = -moveDEC;
    LOGV3(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersIn: Adjusted Target Step pos RA: %f, DEC: %f", moveRA, moveDEC);
  }
  // If we reach the limit in the negative direction...
  else if (moveRA < -RALimit) {
    LOGV2(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersIn: RA is past -limit: %f, DEC: %f", -RALimit);
    // ... turn both RA and DEC axis around
#if RA_Stepper_TYPE == 0
    moveRA += long(12.0f * stepsPerSiderealHour / 2);
#else
    moveRA += long(12.0f * stepsPerSiderealHour);
#endif
    moveDEC = -moveDEC;
    LOGV1(DEBUG_MOUNT_VERBOSE,"Mount::CalcSteppersPost: Adjusted Target. Moved RA, inverted DEC");
  }

  LOGV3(DEBUG_MOUNT,"Mount::CalcSteppersPost: Target Steps RA: %f, DEC: %f", -moveRA, moveDEC);
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
  LOGV3(DEBUG_MOUNT,"Mount::MoveSteppersTo: RA  From: %l  To: %f", _stepperRA->currentPosition(), targetRA);
  LOGV3(DEBUG_MOUNT,"Mount::MoveSteppersTo: DEC From: %l  To: %f", _stepperDEC->currentPosition(), targetDEC);

  if ((_stepperRA->currentPosition() - targetRA) > 0) {
    LOGV1(DEBUG_MOUNT_VERBOSE,"Mount::MoveSteppersTo: Needs backlash correction!");
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
#if HEADLESS_CLIENT == 0

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
#if SUPPORT_SERIAL_CONTROL == 1
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
#if HEADLESS_CLIENT == 0
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
    LOGV1(DEBUG_MOUNT_VERBOSE,"DECString: TARGET!");
    dec = _targetDEC;
  }
  else {
    LOGV1(DEBUG_MOUNT_VERBOSE,"DECString: CURRENT!");
    dec = DegreeTime(currentDEC());
  }
  LOGV2(DEBUG_MOUNT_VERBOSE,"DECString: Precheck  : %s", dec.ToString());
  dec.checkHours();
  LOGV2(DEBUG_MOUNT_VERBOSE,"DECString: Postcheck : %s", dec.ToString());

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

/////////////////////////////////
//
// StartFindingHome
//
/////////////////////////////////
// Automatically home the mount. Only with TMC2209 in UART mode
#if RA_Driver_TYPE == 3 && DEC_Driver_TYPE == 3 && USE_AUTOHOME == 1

void Mount::StartFindingHomeDEC()  {
  _driverDEC->SGTHRS(10);
  _driverDEC->microsteps(16);
  _driverDEC->rms_current(700);
  

  setManualSlewMode(true);
  _mountStatus |= STATUS_FINDING_HOME;
  _stepperDEC->setMaxSpeed(3000);
  _stepperDEC->setSpeed(-3000);
}

void Mount::finishFindingHomeDEC() 
{  
  _stepperDEC->stop();   
   setManualSlewMode(false);
   _stepperDEC->setMaxSpeed(1000);
  _stepperDEC->setSpeed(1000);
  //_stepperDEC->move(2350);
  _stepperDEC->move(100);
  while (_stepperDEC->run());
  
  setManualSlewMode(false);
  //setHome();
  
  //_mountStatus |= STATUS_TRACKING;
  delay(100);
  
  StartFindingHomeRA(); 
  
}

void Mount::StartFindingHomeRA()  {
  _driverRA->SGTHRS(50);
  _driverRA->rms_current(1000);
  _driverRA->microsteps(FULLSTEP);
  _driverRA->semin(0);  // turn off coolstep
  _driverRA->semin(5);
  //_driverRA->TCOOLTHRS(0xFF);  // turn autocurrent threshold down to prevent false reading
  
  setManualSlewMode(true);
  //_mountStatus |= STATUS_FINDING_HOME;
  
  _stepperRA->setMaxSpeed(500);
  _stepperRA->setAcceleration(500);
  _stepperRA->setSpeed(-500);
}

void Mount::finishFindingHomeRA() 
{
  
  _stepperRA->stop();   
  
  _stepperRA->setSpeed(1000);
  //_stepperRA->move(15850.0);
  setManualSlewMode(false);
  _stepperRA->move(1000);  
  
  while (_stepperRA->run());
  

   //setManualSlewMode(false);
   
   
   startSlewing(TRACKING);
   setHome();

}
#endif
