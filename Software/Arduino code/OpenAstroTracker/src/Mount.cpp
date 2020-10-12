#include "InterruptCallback.hpp"

#include "LcdMenu.hpp"
#include "Mount.hpp"
#include "Utility.hpp"
#include "EPROMStore.hpp"
#include "../Configuration_adv.hpp"
#include "../Configuration_pins.hpp"
#include "inc/Globals.hpp"

//mountstatus
#define STATUS_PARKED              0B0000000000000000
#define STATUS_SLEWING             0B0000000000000010
#define STATUS_SLEWING_TO_TARGET   0B0000000000000100
#define STATUS_SLEWING_FREE        0B0000000000000010
#define STATUS_SLEWING_MANUAL      0B0000000100000000
#define STATUS_TRACKING            0B0000000000001000
#define STATUS_PARKING             0B0000000000010000
#define STATUS_PARKING_POS         0B0001000000000000
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

// EPROM constants
#define EEPROM_MAGIC_MASK                  0xFE00    // If these bits are set to 0xBE00, something has been written to the EEPROM
#define EEPROM_MAGIC_EXTENDED_MASK         0xFF00    // If these bits are set to 0xBF00, an extended value has been written to the EEPROM
#define EEPROM_MAGIC_MARKER                0xBE00
#define EEPROM_MAGIC_EXTENDED_MARKER       0xBF00

// The markjer bits for the first 8 values stored in EEPROM.
#define EEPROM_RA_STEPS_BIT                0x0001
#define EEPROM_DEC_STEPS_BIT               0x0002
#define EEPROM_SPEED_FACTOR_BIT            0x0004
#define EEPROM_BACKLASH_STEPS_BIT          0x0008
#define EEPROM_LATITUDE_BIT                0x0010
#define EEPROM_LONGITUDE_BIT               0x0020
#define EEPROM_PITCH_OFFSET_BIT            0x0040
#define EEPROM_ROLL_OFFSET_BIT             0x0080

// The mask bits to test what is stored
#define EEPROM_MAGIC_MASK_RA_STEPS         (EEPROM_MAGIC_MASK | EEPROM_RA_STEPS_BIT) 
#define EEPROM_MAGIC_MASK_DEC_STEPS        (EEPROM_MAGIC_MASK | EEPROM_DEC_STEPS_BIT) 
#define EEPROM_MAGIC_MASK_SPEED_FACTOR     (EEPROM_MAGIC_MASK | EEPROM_SPEED_FACTOR_BIT) 
#define EEPROM_MAGIC_MASK_BACKLASH_STEPS   (EEPROM_MAGIC_MASK | EEPROM_BACKLASH_STEPS_BIT) 
#define EEPROM_MAGIC_MASK_LATITUDE         (EEPROM_MAGIC_MASK | EEPROM_LATITUDE_BIT) 
#define EEPROM_MAGIC_MASK_LONGITUDE        (EEPROM_MAGIC_MASK | EEPROM_LONGITUDE_BIT) 
#define EEPROM_MAGIC_MASK_PITCH_OFFSET     (EEPROM_MAGIC_MASK | EEPROM_PITCH_OFFSET_BIT) 
#define EEPROM_MAGIC_MASK_ROLL_OFFSET      (EEPROM_MAGIC_MASK | EEPROM_ROLL_OFFSET_BIT) 

// The result bits (after masking with above mask) to indicate what is stored
#define EEPROM_RA_STEPS_MARKER_BIT         (EEPROM_MAGIC_MARKER | EEPROM_RA_STEPS_BIT) 
#define EEPROM_DEC_STEPS_MARKER_BIT        (EEPROM_MAGIC_MARKER | EEPROM_DEC_STEPS_BIT)
#define EEPROM_SPEED_FACTOR_MARKER_BIT     (EEPROM_MAGIC_MARKER | EEPROM_SPEED_FACTOR_BIT) 
#define EEPROM_BACKLASH_STEPS_MARKER_BIT   (EEPROM_MAGIC_MARKER | EEPROM_BACKLASH_STEPS_BIT) 
#define EEPROM_LATITUDE_MARKER_BIT         (EEPROM_MAGIC_MARKER | EEPROM_LATITUDE_BIT) 
#define EEPROM_LONGITUDE_MARKER_BIT        (EEPROM_MAGIC_MARKER | EEPROM_LONGITUDE_BIT) 
#define EEPROM_PITCH_OFFSET_MARKER_BIT     (EEPROM_MAGIC_MARKER | EEPROM_PITCH_OFFSET_BIT) 
#define EEPROM_ROLL_OFFSET_MARKER_BIT      (EEPROM_MAGIC_MARKER | EEPROM_ROLL_OFFSET_BIT) 

// Extended bits
#define EEPROM_PARKING_POS_MARKER_BIT         0x0001

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

Mount* Mount::_instance = nullptr;
Mount Mount::instance() { return *_instance; };

const float siderealDegreesInHour = 14.95902778;
/////////////////////////////////
//
// CTOR
//
/////////////////////////////////
Mount::Mount(int stepsPerRADegree, int stepsPerDECDegree, LcdMenu* lcdMenu) {
   _instance = this;
   
  #if RA_DRIVER_TYPE != DRIVER_TYPE_ULN2003
  _stepsPerRADegree = stepsPerRADegree * SET_MICROSTEPPING; // hier
  #else
  _stepsPerRADegree = stepsPerRADegree;
  #endif
  #if DEC_DRIVER_TYPE != DRIVER_TYPE_ULN2003
  _stepsPerDECDegree = stepsPerDECDegree * DEC_SLEW_MICROSTEPPING;
  #else
  _stepsPerDECDegree = stepsPerDECDegree;
  #endif
  _lcdMenu = lcdMenu;
  _mountStatus = 0;
  _lastDisplayUpdate = 0;
  _stepperWasRunning = false;
  _latitude = 45;
  _longitude = 100;

  #if AZIMUTH_ALTITUDE_MOTORS == 1
  _azAltWasRunning = false;
  #endif

  _totalDECMove = 0;
  _totalRAMove = 0;
  _moveRate = 4;
#if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  _backlashCorrectionSteps = 16;
#else
  _backlashCorrectionSteps = 0;
#endif
  _correctForBacklash = false;
  _slewingToHome = false;
  _slewingToPark = false;
  _raParkingPos  = 0;
  _decParkingPos = 0;
    
  #if USE_GYRO_LEVEL == 1
  _pitchCalibrationAngle = 0;
  _rollCalibrationAngle = 0;
  #endif

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
  // 2 kHz updates (higher frequency interferes with serial communications and complete messes up OATControl communications)
  if (!InterruptCallback::setInterval(0.5f, mountLoop, this))
  {
    LOGV1(DEBUG_MOUNT, F("Mount:: CANNOT setup interrupt timer!"));
  }
#endif // !ESPBOARD

}

/////////////////////////////////
//
// clearConfiguration
//
/////////////////////////////////
void Mount::clearConfiguration()
{
  EPROMStore::update(5, 0);  // CLear the magic marker
  EPROMStore::update(4, 0);  // Clear the flags
  EPROMStore::update(21, 0); // Clear the extendede flags
  EPROMStore::update(22, 0);
}

/////////////////////////////////
//
// readConfiguration
//
/////////////////////////////////
void Mount::readConfiguration()
{
  LOGV1(DEBUG_INFO, F("Mount: Reading configuration data from EEPROM"));
  readPersistentData();
  LOGV1(DEBUG_INFO, F("Mount: Done reading configuration data from EEPROM"));
}
  
/////////////////////////////////
//
// readPersistentData
//
/////////////////////////////////
//
// EEPROM storage location 5 must be 0xBE or 0xBF for the mount to read any data
// Location 4 indicates what has been stored so far: 0000 0000
//                                                   ^^^^ ^^^^
//                                                   |||| ||||
//                    Roll angle offset (19/20) -----+||| ||||
//                   Pitch angle offset (17/18) ------+|| ||||
//                            Longitude (14/15) -------+| ||||
//                             Latitude (12/13) --------+ ||||
//                       Backlash steps (10/11) ----------+|||
//                           Speed factor (0/3) -----------+||
//     DEC stepper motor steps per degree (8/9) ------------+|
//      RA stepper motor steps per degree (6/7) -------------+
//
// If Location 5 is 0xBF, then an extended 16-bit flag is stored in 21/22 and 
// indicates the additional fields that have been stored: 0000 0000 0000 0000
//                                                        ^^^^ ^^^^ ^^^^ ^^^^
//                                                                          |
//     RA (23-26) and DEC (27-30) Parking offsets --------------------------+
//
/////////////////////////////////
void Mount::readPersistentData()
{
  // Read the magic marker byte and state
  uint16_t marker = EPROMStore::readInt16(4, 5);

  LOGV2(DEBUG_INFO, F("Mount: EEPROM: Magic Marker: %x "), marker);

  if ((marker & EEPROM_MAGIC_MASK_RA_STEPS) == EEPROM_RA_STEPS_MARKER_BIT) {
    _stepsPerRADegree = EPROMStore::read(6) + EPROMStore::read(7) * 256;
    LOGV2(DEBUG_INFO,F("Mount: EEPROM: RA Marker OK! RA steps/deg is %d"), _stepsPerRADegree);
  }
  else{
    LOGV1(DEBUG_INFO,F("Mount: EEPROM: No stored value for RA steps"));
  }

  if ((marker & EEPROM_MAGIC_MASK_DEC_STEPS) == EEPROM_DEC_STEPS_MARKER_BIT) {
    _stepsPerDECDegree = EPROMStore::read(8) + EPROMStore::read(9) * 256;
    LOGV2(DEBUG_INFO,F("Mount: EEPROM: DEC Marker OK! DEC steps/deg is %d"), _stepsPerDECDegree);
  }
  else{
    LOGV1(DEBUG_INFO,F("Mount: EEPROM: No stored value for DEC steps"));
  }

  float speed = 1.0;
  if ((marker & EEPROM_MAGIC_MASK_SPEED_FACTOR) == EEPROM_SPEED_FACTOR_MARKER_BIT) {
    int adjust = EPROMStore::read(0) + EPROMStore::read(3) * 256;
    speed = 1.0 + 1.0 * adjust / 10000.0;
    LOGV3(DEBUG_INFO,F("Mount: EEPROM: Speed Marker OK! Speed adjust is %d, speedFactor is %f"), adjust, speed);
  }
  else{
    LOGV1(DEBUG_INFO,F("Mount: EEPROM: No stored value for speed factor"));
  }

  if ((marker & EEPROM_MAGIC_MASK_BACKLASH_STEPS) == EEPROM_BACKLASH_STEPS_MARKER_BIT) {
    _backlashCorrectionSteps = EPROMStore::read(10) + EPROMStore::read(11) * 256;
    LOGV2(DEBUG_INFO,F("Mount: EEPROM: Backlash Steps Marker OK! Backlash correction is %d"), _backlashCorrectionSteps);
  }
  else {
    LOGV1(DEBUG_INFO,F("Mount: EEPROM: No stored value for backlash correction"));
  }

  if ((marker & EEPROM_MAGIC_MASK_LATITUDE) == EEPROM_LATITUDE_MARKER_BIT) {
    _latitude = 1.0f * EPROMStore::readInt16(12, 13) / 100.0f;
    LOGV2(DEBUG_INFO,F("Mount: EEPROM: Latitude Marker OK! Latitude is %f"), _latitude);
  } 
  else {
    LOGV1(DEBUG_INFO,F("Mount: EEPROM: No stored value for latitude"));
  }

  if ((marker & EEPROM_MAGIC_MASK_LONGITUDE) == EEPROM_LONGITUDE_MARKER_BIT) {
    _longitude = 1.0f * EPROMStore::readInt16(14, 15) / 100.0f;
    LOGV2(DEBUG_INFO,F("Mount: EEPROM: Longitude Marker OK! Longitude is %f"), _longitude);
  } 
  else {
    LOGV1(DEBUG_INFO,F("Mount: EEPROM: No stored value for longitude"));
  }

#if USE_GYRO_LEVEL == 1
  if ((marker & EEPROM_MAGIC_MASK_PITCH_OFFSET) == EEPROM_PITCH_OFFSET_MARKER_BIT) {
    uint16_t angleValue = EPROMStore::readInt16(17, 18);
    _pitchCalibrationAngle = (((long)angleValue) - 16384) / 100.0;
    LOGV3(DEBUG_INFO,F("Mount: EEPROM: Pitch Offset Marker OK! Pitch Offset is %x (%f)"), angleValue, _pitchCalibrationAngle);
  }
    else{
    LOGV1(DEBUG_INFO,F("Mount: EEPROM: No stored value for Pitch Offset"));
  }

  if ((marker & EEPROM_MAGIC_MASK_ROLL_OFFSET) == EEPROM_ROLL_OFFSET_MARKER_BIT) {
    uint16_t angleValue = EPROMStore::readInt16(19,20);
    _rollCalibrationAngle = (((long)angleValue) - 16384) / 100.0;
    LOGV3(DEBUG_INFO,F("Mount: EEPROM: Roll Offset Marker OK! Roll Offset is %x (%f)"), angleValue, _rollCalibrationAngle);
  }
  else {
    LOGV1(DEBUG_INFO,F("Mount: EEPROM: No stored value for Roll Offset"));
  }
#endif

  if ((marker & EEPROM_MAGIC_EXTENDED_MASK) == EEPROM_MAGIC_EXTENDED_MARKER) {
    LOGV2(DEBUG_INFO,F("Mount: EEPROM: Magic Marker is %x, reading extended"), marker);
    int16_t nextMarker = EPROMStore::readInt16(21,22);
    LOGV2(DEBUG_INFO,F("Mount: EEPROM: ExtendedMarker is %x"), nextMarker);
    if (nextMarker & EEPROM_PARKING_POS_MARKER_BIT){
      _raParkingPos = EPROMStore::readInt32(23); // 23-26
      _decParkingPos = EPROMStore::readInt32(27); // 27-30
      LOGV3(DEBUG_INFO,F("Mount: EEPROM: Parking position read as R:%l, D:%l"), _raParkingPos, _decParkingPos);
    }
  } 
  else {
    LOGV1(DEBUG_INFO,F("Mount: EEPROM: No ExtendedMarker present"));
  }

  setSpeedCalibration(speed, false);
}

/////////////////////////////////
//
// writePersistentData
//
/////////////////////////////////
void Mount::writePersistentData(int which, long val)
{
  uint8_t flag = 0x00;
  int16_t extendedFlag  = 0x0000;
  int loByteLocation = 0;
  int hiByteLocation = 0;
  bool writeExtended = false;

  // If we're written something before...
  uint16_t magicMarker = EPROMStore::read(5) * 256 + EPROMStore::read(4);
  LOGV3(DEBUG_INFO,F("Mount: EEPROM Write(%d): Marker is %x"), which, magicMarker);
  if ((magicMarker & EEPROM_MAGIC_MASK) == EEPROM_MAGIC_MARKER) {
    // ... read the current state ...
    flag = EPROMStore::read(4);
    if ((magicMarker & EEPROM_MAGIC_EXTENDED_MASK) == EEPROM_MAGIC_EXTENDED_MARKER) {
      extendedFlag = EPROMStore::readInt16(21, 22);
      LOGV3(DEBUG_INFO,F("Mount: EEPROM Write: Marker is 0xBF, extended flag is %x (%d)"), extendedFlag, extendedFlag);
    }
    else{
     LOGV3(DEBUG_INFO,F("Mount: EEPROM Write: Marker is 0xBE, flag is %x (%d)"), flag, flag);
    }
  }
  switch (which) {
    case EEPROM_RA:
    {
      // ... set bit 0 to indicate RA value has been written to 6/7
      flag |= EEPROM_RA_STEPS_BIT;
      loByteLocation = 6;
      hiByteLocation = 7;
      LOGV2(DEBUG_INFO,F("Mount: EEPROM Write: Updating RA steps to %d"), val);
    }
    break;

    case EEPROM_DEC:
    {
      // ... set bit 1 to indicate DEC value has been written to 8/9
      flag |= EEPROM_DEC_STEPS_BIT;
      loByteLocation = 8;
      hiByteLocation = 9;
      LOGV2(DEBUG_INFO,F("Mount: EEPROM Write: Updating DEC steps to %d"), val);
    }
    break;

    case EEPROM_SPEED:
    {
      // ... set bit 3 to indicate speed factor value has been written to 0/3
      flag |= EEPROM_SPEED_FACTOR_BIT;
      loByteLocation = 0;
      hiByteLocation = 3;
      LOGV2(DEBUG_INFO,F("Mount: EEPROM Write: Updating Speed factor to %d"), val);
    }
    break;

    case EEPROM_BACKLASH:
    {
      // ... set bit 4 to indicate backlash has been written to 10/11
      flag |= EEPROM_BACKLASH_STEPS_BIT;
      loByteLocation = 10;
      hiByteLocation = 11;
      LOGV2(DEBUG_INFO,F("Mount: EEPROM Write: Updating Backlash to %d"), val);
    }
    break;

    case EEPROM_LATITUDE:
    {
      // ... set bit 5 to indicate Latitude x100  has been written to 12/13
      flag |= EEPROM_LATITUDE_BIT;
      loByteLocation = 12;
      hiByteLocation = 13;
      LOGV2(DEBUG_INFO,F("Mount: EEPROM Write: Updating Latitude to %d"), val);
    }
    break;

    case EEPROM_LONGITUDE:
    {
      // ... set bit 6 to indicate Longitude x100  has been written to 14/15
      flag |= EEPROM_LONGITUDE_BIT;
      loByteLocation = 14;
      hiByteLocation = 15;
      LOGV2(DEBUG_INFO,F("Mount: EEPROM Write: Updating Longitude to %d"), val);
    }
    break;

    case EEPROM_PITCH_OFFSET:
    {
      // ... set bit 7 to indicate pitch offset angle value has been written to 17/18
      flag |= EEPROM_PITCH_OFFSET_BIT;
      loByteLocation = 17;
      hiByteLocation = 18;
      LOGV2(DEBUG_INFO,F("Mount: EEPROM Write: Updating Pitch Offset to %d"), val);
    }
    break;

    case EEPROM_ROLL_OFFSET:
    {
      // ... set bit 8 to indicate pitch offset angle value has been written to 19/20
      flag |= EEPROM_ROLL_OFFSET_BIT;
      loByteLocation = 19;
      hiByteLocation = 20;
      LOGV2(DEBUG_INFO,F("Mount: EEPROM Write: Updating Roll Offset to %d"), val);
    }
    break;

    case EEPROM_RA_PARKING_POS:
    case EEPROM_DEC_PARKING_POS:
    {
      // ... set bit 8 to indicate pitch offset angle value has been written to 19/20
      writeExtended = true;
      extendedFlag |= EEPROM_PARKING_POS_MARKER_BIT;
      if (which == EEPROM_RA_PARKING_POS ){
        EPROMStore::updateInt32(23, val);
        LOGV2(DEBUG_INFO,F("Mount: EEPROM Write: Updating RA Parking Pos to %l at 23-26"), val);
      }
      else{
        EPROMStore::updateInt32(27, val);
        LOGV2(DEBUG_INFO,F("Mount: EEPROM Write: Updating DEC Parking Pos to %l at 27-30"), val);
      }
    }
    break;
  }


  if (writeExtended) {
    LOGV3(DEBUG_INFO,F("Mount: EEPROM Write: New Marker is 0xBF, extended flag is %x (%d)"), extendedFlag, extendedFlag);
    EPROMStore::update(5, EEPROM_MAGIC_EXTENDED_MARKER >> 8);
    EPROMStore::updateInt16(21, 22, extendedFlag);
  }
  else {
    LOGV4(DEBUG_INFO,F("Mount: EEPROM Write: New Marker is %d, flag is %x (%d)"), magicMarker, flag, flag);
    EPROMStore::update(4, flag);
    EPROMStore::update(5, magicMarker >> 8);

    EPROMStore::update(loByteLocation, val & 0x00FF);
    EPROMStore::update(hiByteLocation, (val >> 8) & 0x00FF);
    LOGV5(DEBUG_INFO,F("Mount: EEPROM Write: Wrote %x to %d and %x to %d"), val & 0x00FF, loByteLocation, (val >> 8) & 0x00FF, hiByteLocation);
  }
}

/////////////////////////////////
//
// configureRAStepper
//
/////////////////////////////////
#if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
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
  _stepperTRK = new AccelStepper(HALFSTEP_MODE, pin4, pin3, pin2, pin1);
#else
  _stepperTRK = new AccelStepper(HALFSTEP_MODE, pin1, pin2, pin3, pin4);
#endif
  _stepperTRK->setMaxSpeed(10);
  _stepperTRK->setAcceleration(2500);
}
#endif

#if RA_STEPPER_TYPE == STEPPER_TYPE_NEMA17
void Mount::configureRAStepper(byte stepMode, byte pin1, byte pin2, int maxSpeed, int maxAcceleration)
{
  _stepperRA = new AccelStepper(stepMode, pin1, pin2);
  _stepperRA->setMaxSpeed(maxSpeed);
  _stepperRA->setAcceleration(maxAcceleration);
  _maxRASpeed = maxSpeed;
  _maxRAAcceleration = maxAcceleration;

  // Use another AccelStepper to run the RA motor as well. This instance tracks earths rotation.
  _stepperTRK = new AccelStepper(DRIVER_MODE, pin1, pin2);
  _stepperTRK->setMaxSpeed(500);
  _stepperTRK->setAcceleration(10000);

  #if NORTHERN_HEMISPHERE != 1
  _stepperRA->setPinsInverted(true, false, false);
  _stepperTRK->setPinsInverted(true, false, false);
  #endif
  
  #if RA_INVERT_DIR == 1
  _stepperRA->setPinsInverted(true, false, false);
  _stepperTRK->setPinsInverted(true, false, false);
  #endif
}
#endif

/////////////////////////////////
//
// configureDECStepper
//
/////////////////////////////////
#if DEC_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
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

#if AZIMUTH_ALTITUDE_MOTORS == 1
void Mount::configureAzStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration)
{
  _stepperAZ = new AccelStepper(HALFSTEP_MODE, pin1, pin2, pin3, pin4);
  _stepperAZ->setSpeed(0);
  _stepperAZ->setMaxSpeed(maxSpeed);
  _stepperAZ->setAcceleration(maxAcceleration);
}

void Mount::configureAltStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration)
{
  _stepperALT = new AccelStepper(FULLSTEP_MODE, pin1, pin2, pin3, pin4);
  _stepperALT->setSpeed(0);
  _stepperALT->setMaxSpeed(maxSpeed);
  _stepperALT->setAcceleration(maxAcceleration);
}
#endif

#if DEC_STEPPER_TYPE == STEPPER_TYPE_NEMA17
void Mount::configureDECStepper(byte stepMode, byte pin1, byte pin2, int maxSpeed, int maxAcceleration)
{
  _stepperDEC = new AccelStepper(stepMode, pin1, pin2);
  _stepperDEC->setMaxSpeed(maxSpeed);
  _stepperDEC->setAcceleration(maxAcceleration);
  _maxDECSpeed = maxSpeed;
  _maxDECAcceleration = maxAcceleration;
  
  #if DEC_INVERT_DIR == 1
  _stepperDEC->setPinsInverted(true, false, false);
  #endif
}
#endif

/////////////////////////////////
//
// configureRAdriver
// TMC2209 UART only
/////////////////////////////////
#if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
void Mount::configureRAdriver(HardwareSerial *serial, float rsense, byte driveraddress, int rmscurrent, int stallvalue)
{
  _driverRA = new TMC2209Stepper(serial, rsense, driveraddress);
  _driverRA->begin();
  #if RA_AUDIO_FEEDBACK == 1
  _driverRA->en_spreadCycle(1);
  #endif
  _driverRA->toff(4);
  _driverRA->blank_time(24);
  _driverRA->rms_current(rmscurrent);
  _driverRA->microsteps(TRACKING_MICROSTEPPING);
  _driverRA->fclktrim(4);
  _driverRA->TCOOLTHRS(0xFFFFF);  //xFFFFF);
  _driverRA->ihold(1); // its save to assume that the only time RA stands still is during parking and the current can be limited to a minimum
  //_driverRA->semin(2);
  //_driverRA->semax(5);
  //_driverRA->sedn(0b01);
  //_driverRA->SGTHRS(10);
  _driverRA->irun(31);
  
}
#endif

/////////////////////////////////
//
// configureDECdriver
// TMC2209 UART only
/////////////////////////////////
#if DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
void Mount::configureDECdriver(HardwareSerial *serial, float rsense, byte driveraddress, int rmscurrent, int stallvalue)
{
  _driverDEC = new TMC2209Stepper(serial, rsense, driveraddress);
  _driverDEC->begin();
  _driverDEC->blank_time(24);
  #if DEC_AUDIO_FEEDBACK == 1
  _driverDEC->en_spreadCycle(1);
  #endif
  _driverDEC->rms_current(rmscurrent);
  _driverDEC->microsteps(DEC_SLEW_MICROSTEPPING);
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
  LOGV3(DEBUG_MOUNT, F("Mount: Updating speed calibration from %f to %f"), _trackingSpeedCalibration , val);
  _trackingSpeedCalibration = val;

  LOGV2(DEBUG_MOUNT, F("Mount: Current tracking speed is %f steps/sec"), _trackingSpeed);

  // The tracker simply needs to rotate at 15degrees/hour, adjusted for sidereal
  // time (i.e. the 15degrees is per 23h56m04s. 86164s/86400 = 0.99726852. 3590/3600 is the same ratio) So we only go 15 x 0.99726852 in an hour.
  #if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
  _trackingSpeed = _trackingSpeedCalibration * _stepsPerRADegree * TRACKING_MICROSTEPPING * siderealDegreesInHour / (3600.0f * SET_MICROSTEPPING);
  #else
  _trackingSpeed = _trackingSpeedCalibration * _stepsPerRADegree * siderealDegreesInHour / 3600.0f;
  #endif
  LOGV2(DEBUG_MOUNT, F("Mount: New tracking speed is %f steps/sec"), _trackingSpeed);

  if (saveToStorage) {
    val = (val - 1.0) * 10000;
    if (val > 32766) val = 32766;
    if (val < -32766) val = -32766;
    writePersistentData(EEPROM_SPEED, (int)floor(val));
  }

  // If we are currently tracking, update the speed.
  if (isSlewingTRK()) {
    _stepperTRK->setSpeed(_trackingSpeed);
  }
}

#if USE_GYRO_LEVEL == 1
/////////////////////////////////
//
// getPitchCalibrationAngle
//
// The pitch value at which the mount is level
/////////////////////////////////
float Mount::getPitchCalibrationAngle()
{
  return _pitchCalibrationAngle;
}

/////////////////////////////////
//
// setPitchCalibrationAngle
//
/////////////////////////////////
void Mount::setPitchCalibrationAngle(float angle)
{
    uint16_t angleValue = (angle * 100) + 16384;
    writePersistentData(EEPROM_PITCH_OFFSET, angleValue);
    _pitchCalibrationAngle = angle;
}

/////////////////////////////////
//
// getRollCalibration
//
// The roll value at which the mount is level
/////////////////////////////////
float Mount::getRollCalibrationAngle()
{
  return _rollCalibrationAngle;
}

/////////////////////////////////
//
// setRollCalibration
//
/////////////////////////////////
void Mount::setRollCalibrationAngle(float angle)
{
    uint16_t angleValue = (angle * 100) + 16384;
    writePersistentData(EEPROM_ROLL_OFFSET, angleValue);
    _rollCalibrationAngle = angle;
}
#endif

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
    writePersistentData(EEPROM_DEC, steps);
    _stepsPerDECDegree = steps;

  }
  else if (which == RA_STEPS) {
    writePersistentData(EEPROM_RA , steps);
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
// getMountHardwareInfo
//
/////////////////////////////////
String Mount::getMountHardwareInfo()
{
  String ret = "Unknown";
  #if defined(ESP8266) 
    ret = "ESP8266,";
  #elif defined(ESP32)
    ret = "ESP32,";
  #elif defined(__AVR_ATmega2560__)
    ret = "Mega,";
  #elif defined (__AVR_ATmega328P__)
    ret = "Uno,";
  #endif

  #if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
    ret += "28BYJ|";
  #elif RA_STEPPER_TYPE == STEPPER_TYPE_NEMA17
    ret += "NEMA|";
  #else
    ret += "?|";
  #endif
  ret += String(RA_PULLEY_TEETH)+"|";
  ret += String(RA_STEPPER_SPR)+",";

  #if DEC_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
    ret += "28BYJ|";
  #elif DEC_STEPPER_TYPE == STEPPER_TYPE_NEMA17
    ret += "NEMA|";
  #else
    ret += "?|";
  #endif

  ret += String(DEC_PULLEY_TEETH)+"|";
  ret += String(DEC_STEPPER_SPR)+",";

  #if USE_GPS == 1
    ret += "GPS,";
  #else
    ret += "NO_GPS,";
  #endif

  #if AZIMUTH_ALTITUDE_MOTORS == 1
    ret += "AUTO_AZ_ALT,";
  #else
    ret += "NO_AZ_ALT,";
  #endif

  #if USE_GYRO_LEVEL == 1
    ret += "GYRO,";
  #else
    ret += "NO_GYRO,";
  #endif

  return ret;
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
  writePersistentData(EEPROM_BACKLASH, steps);
}

/////////////////////////////////
//
// setSlewRate
//
/////////////////////////////////
void Mount::setSlewRate(int rate) 
{
  _moveRate = clamp(rate, 1, 4);
  float speedFactor[] = { 0, 0.05, 0.2, 0.5, 1.0};
  LOGV3(DEBUG_MOUNT,F("Mount::setSlewRate: rate is %d -> %f"),_moveRate , speedFactor[_moveRate ]);
  _stepperDEC->setMaxSpeed(speedFactor[_moveRate ] * _maxDECSpeed);
  _stepperRA->setMaxSpeed(speedFactor[_moveRate ] * _maxRASpeed);
  LOGV3(DEBUG_MOUNT,F("Mount::setSlewRate: new speeds are RA: %f  DEC: %f"),_stepperRA->maxSpeed(), _stepperDEC->maxSpeed());
}

/////////////////////////////////
//
// setHA
//
/////////////////////////////////
void Mount::setHA(const DayTime& haTime) {
  LOGV2(DEBUG_MOUNT,F("Mount: setHA:  HA is %s"), haTime.ToString());
  DayTime lst = DayTime(POLARIS_RA_HOUR, POLARIS_RA_MINUTE, POLARIS_RA_SECOND);
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
  // LOGV1(DEBUG_MOUNT_VERBOSE, F("Mount: Get HA."));
  // LOGV2(DEBUG_MOUNT_VERBOSE, F("Mount: Polaris adjust: %s"), DayTime(POLARIS_RA_HOUR, POLARIS_RA_MINUTE, POLARIS_RA_SECOND).ToString());
  DayTime ha = _LST;
  // LOGV2(DEBUG_MOUNT_VERBOSE, F("Mount: LST: %s"), _LST.ToString());
  ha.subtractTime(DayTime(POLARIS_RA_HOUR, POLARIS_RA_MINUTE, POLARIS_RA_SECOND));
  LOGV2(DEBUG_MOUNT, F("Mount: GetHA: LST-Polaris is HA %s"), ha.ToString());
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
  LOGV2(DEBUG_MOUNT,F("Mount: Set LST and ZeroPosRA to: %s"), _LST.ToString());
}

/////////////////////////////////
//
// setLatitude
//
/////////////////////////////////
void Mount::setLatitude(float lat) {
  _latitude = lat;
  writePersistentData(EEPROM_LATITUDE, round(lat * 100));
}

/////////////////////////////////
//
// setLongitude
//
/////////////////////////////////
void Mount::setLongitude(float lon) {
  _longitude = lon;
  writePersistentData(EEPROM_LONGITUDE, round(lon * 100));
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
  #if RA_DRIVER_TYPE == DRIVER_TYPE_ULN2003
  float hourPos = 2.0 * -_stepperRA->currentPosition() / stepsPerSiderealHour;
  #else
  float hourPos =  -_stepperRA->currentPosition() / stepsPerSiderealHour;
  #endif
  LOGV4(DEBUG_MOUNT_VERBOSE,F("CurrentRA: Steps/h    : %s (%d x %s)"), String(stepsPerSiderealHour, 2).c_str(), _stepsPerRADegree, String(siderealDegreesInHour, 5).c_str());
  LOGV2(DEBUG_MOUNT_VERBOSE,F("CurrentRA: RA Steps   : %d"), _stepperRA->currentPosition());
  LOGV2(DEBUG_MOUNT_VERBOSE,F("CurrentRA: POS        : %s"), String(hourPos).c_str());
  hourPos += _zeroPosRA.getTotalHours();
  // LOGV2(DEBUG_MOUNT_VERBOSE,F("CurrentRA: ZeroPos    : %s"), _zeroPosRA.ToString());
  // LOGV2(DEBUG_MOUNT_VERBOSE,F("CurrentRA: POS (+zp)  : %s"), DayTime(hourPos).ToString());

  bool flipRA = NORTHERN_HEMISPHERE ?
    _stepperDEC->currentPosition() < 0
    : _stepperDEC->currentPosition() > 0;
  if (flipRA)
  {
    hourPos += 12;
    if (hourPos > 24) hourPos -= 24;
    // LOGV2(DEBUG_MOUNT_VERBOSE,F("CurrentRA: RA (+12h): %s"), DayTime(hourPos).ToString());
  }

  // Make sure we are normalized
  if (hourPos < 0) hourPos += 24;
  if (hourPos > 24) hourPos -= 24;

  // LOGV2(DEBUG_MOUNT_VERBOSE,F("CurrentRA: RA Pos  -> : %s"), DayTime(hourPos).ToString());
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
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("CurrentDEC: Steps/deg  : %d"), _stepsPerDECDegree);
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("CurrentDEC: DEC Steps  : %d"), _stepperDEC->currentPosition());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("CurrentDEC: POS        : %s"), String(degreePos).c_str());

  if (degreePos > 0)
  {
    degreePos = -degreePos;
    //LOGV1(DEBUG_MOUNT_VERBOSE,F("CurrentDEC: Greater Zero, flipping."));
  }

  //LOGV2(DEBUG_MOUNT_VERBOSE,F("CurrentDEC: POS      : %s"), DegreeTime(degreePos).ToString());
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
  LOGV3(DEBUG_MOUNT, F("Mount: Sync Stepper Position is RA: %d and DEC: %d"), targetRA, targetDEC);
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
  #if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
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
  LOGV3(DEBUG_MOUNT, "Mount: RA Dist: %d,   DEC Dist: %d", _stepperRA->distanceToGo(), _stepperDEC->distanceToGo());
  #if RA_STEPPER_TYPE == STEPPER_TYPE_NEMA17  // tracking while slewing causes audible lagging
  if (_stepperRA->distanceToGo() > 0) {
    // Only stop tracking if we're actually going to slew somewhere else, otherwise the 
    // mount::loop() code won't detect the end of the slewing operation...
    stopSlewing(TRACKING);
  }
  #endif																					  
}

/////////////////////////////////
//
// stopGuiding
//
/////////////////////////////////
void Mount::stopGuiding() {
  _stepperDEC->stop();
  /*while (_stepperDEC->isRunning()) {
    _stepperDEC->run();
  }*/

  _stepperDEC->setMaxSpeed(_maxDECSpeed);
  _stepperDEC->setAcceleration(_maxDECAcceleration);
  #if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  _stepperTRK->setMaxSpeed(10);
  #else
  _stepperTRK->setMaxSpeed(500);
  #endif
  #if DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
    _driverDEC->microsteps(DEC_SLEW_MICROSTEPPING);
  #endif
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
  #if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  float raTrackingSpeed = _stepsPerRADegree * siderealDegreesInHour / 3600.0f;
  #else
  float raTrackingSpeed = 1.0 * _stepsPerRADegree * TRACKING_MICROSTEPPING * siderealDegreesInHour / (3600.0f * SET_MICROSTEPPING);
  #endif

  // TODO: Do we need to track how many steps the steppers took and add them to the GoHome calculation?
  // If so, we need to remember where we were when we started the guide pulse. Then at the end,
  // we can calculate the difference.
  // long raPos = _stepperTRK->currentPosition();
  // long decPos = _stepperDEC->currentPosition();

  switch (direction) {
    case NORTH:
    #if DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
    _driverDEC->microsteps(DEC_GUIDE_MICROSTEPPING);
    #endif
    _stepperDEC->setAcceleration(2500);
    #if DEC_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
    _stepperDEC->setMaxSpeed(decTrackingSpeed * (1.0 + 0.2));
    _stepperDEC->setSpeed(decTrackingSpeed * 1.0);
    #else // NEMA
    _stepperDEC->setMaxSpeed(decTrackingSpeed * (DEC_PULSE_MULTIPLIER + 0.2));
    _stepperDEC->setSpeed(decTrackingSpeed * DEC_PULSE_MULTIPLIER);
    #endif
    _mountStatus |= STATUS_GUIDE_PULSE | STATUS_GUIDE_PULSE_DEC;
    break;

    case SOUTH:
    #if DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
    _driverDEC->microsteps(DEC_GUIDE_MICROSTEPPING);
    #endif
    _stepperDEC->setAcceleration(2500);
    #if DEC_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
    _stepperDEC->setMaxSpeed(decTrackingSpeed * (1.0 + 0.2));
    _stepperDEC->setSpeed(-decTrackingSpeed * 1.0);
    #else // NEMA
    _stepperDEC->setMaxSpeed(decTrackingSpeed * (DEC_PULSE_MULTIPLIER + 0.2));
    _stepperDEC->setSpeed(-decTrackingSpeed * DEC_PULSE_MULTIPLIER);
    #endif
    _mountStatus |= STATUS_GUIDE_PULSE | STATUS_GUIDE_PULSE_DEC;
    break;

    case WEST:
    
    #if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
    _stepperTRK->setMaxSpeed(raTrackingSpeed * 2.2 );
    _stepperTRK->setSpeed(raTrackingSpeed * 2.0);
    #else  // NEMA
    _stepperTRK->setMaxSpeed(raTrackingSpeed * (RA_PULSE_MULTIPLIER + 0.2 ));
    _stepperTRK->setSpeed(raTrackingSpeed * RA_PULSE_MULTIPLIER);
    #endif
    _mountStatus |= STATUS_GUIDE_PULSE | STATUS_GUIDE_PULSE_RA;
    break;

    case EAST:
    #if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
      _stepperTRK->setMaxSpeed(raTrackingSpeed * 2.2 );
      _stepperTRK->setSpeed(0);
    #else // NEMA
      // Not sure why we don't stop tracking with NEMAs as is customary.....
      _stepperTRK->setMaxSpeed(raTrackingSpeed * (RA_PULSE_MULTIPLIER + 0.2));
      _stepperTRK->setSpeed(raTrackingSpeed * (RA_PULSE_MULTIPLIER - 1));
    #endif
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
  #if AZIMUTH_ALTITUDE_MOTORS == 1
  else if (which == AZIMUTH_STEPS) {
    float curAzSpeed = _stepperAZ->speed();

    // If we are changing directions or asking for a stop, do a stop
    if ((signbit(speed) != signbit(curAzSpeed)) || (speed == 0))
    {
      _stepperAZ->stop();
      while (_stepperAZ->isRunning()){
        loop();
      }
    }

    // Are we starting a move or changing speeds?
    if (speed != 0) {
      _stepperAZ->enableOutputs();
      _stepperAZ->setSpeed(speed);
      _stepperAZ->move(speed * 100000);
    } // Are we stopping a move?
    else if (speed == 0) {
      _stepperAZ->disableOutputs();
    }
  }
  else if (which == ALTITUDE_STEPS) {
    float curAltSpeed = _stepperALT->speed();

    // If we are changing directions or asking for a stop, do a stop
    if ((signbit(speed) != signbit(curAltSpeed)) || (speed == 0))
    {
      _stepperALT->stop();
      while (_stepperALT->isRunning()){
        loop();
      }
    }

    // Are we starting a move or changing speeds?
    if (speed != 0) {
      _stepperALT->enableOutputs();
      _stepperALT->setSpeed(speed);
      _stepperALT->move(speed * 100000);
    } // Are we stopping a move?
    else if (speed == 0) {
      _stepperALT->disableOutputs();
    }
  }
  #endif
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

#if AZIMUTH_ALTITUDE_MOTORS == 1
/////////////////////////////////
//
// isRunningAZ
//
/////////////////////////////////
bool Mount::isRunningAZ() const {
  return _stepperAZ->isRunning();
}

/////////////////////////////////
//
// isRunningALT
//
/////////////////////////////////
bool Mount::isRunningALT() const {
  return _stepperALT->isRunning();
}

/////////////////////////////////
//
// getAltAzPositions
//
/////////////////////////////////
void Mount::getAltAzPositions(long * altSteps, long* azSteps, float* altDeltaSecs, float*  azDeltaSecs){
  if (altSteps != nullptr) {
    *altSteps = _stepperALT->currentPosition();
  }
  if (azSteps != nullptr) {
    *azSteps = _stepperAZ->currentPosition();
  }
  if (altDeltaSecs != nullptr) {
    *altDeltaSecs = _stepperALT->currentPosition() * ALTITUDE_ARC_SECONDS_PER_STEP;
  }
  if (azDeltaSecs != nullptr) {
    *azDeltaSecs = _stepperAZ->currentPosition() * AZIMUTH_ARC_SECONDS_PER_STEP;
  }
}
/////////////////////////////////
//
// moveBy
//
/////////////////////////////////
void Mount::moveBy(int direction, float arcMinutes)
{
    if (direction == AZIMUTH_STEPS) {
      enableAzAltMotors();
      int stepsToMove = 2.0f * arcMinutes * AZIMUTH_STEPS_PER_ARC_MINUTE;
      _stepperAZ->move(stepsToMove);
    }
    else if (direction == ALTITUDE_STEPS) {
      enableAzAltMotors();
      int stepsToMove = arcMinutes * ALTITUDE_STEPS_PER_ARC_MINUTE;
      _stepperALT->move(stepsToMove);
    }
}

/////////////////////////////////
//
// disableAzAltMotors
//
/////////////////////////////////
void Mount::disableAzAltMotors() {
  _stepperALT->stop();
  _stepperAZ->stop();
  while (_stepperALT->isRunning() || _stepperAZ->isRunning()){
    loop();
  }
  _stepperALT->disableOutputs();
  _stepperAZ->disableOutputs();
}

/////////////////////////////////
//
// disableAzAltMotors
//
/////////////////////////////////
void Mount::enableAzAltMotors() {
  _stepperALT->enableOutputs();
  _stepperAZ->enableOutputs();
}

#endif

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
  else if ((_mountStatus & STATUS_PARKING) || (_mountStatus & STATUS_PARKING_POS)) {
    status = "Parking,";
  }
  else if (isGuiding()) {
    status = "Guiding,";
  }
  else if (isFindingHome()) {
    status = "Homing,";
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

  String disp = "-----,";
  if (_mountStatus & STATUS_SLEWING) {
    byte slew = slewStatus();
    if (slew & SLEWING_RA) disp[0] = _stepperRA->speed() < 0 ? 'R' : 'r';
    if (slew & SLEWING_DEC) disp[1] = _stepperDEC->speed() < 0 ? 'D' : 'd';
    if (slew & SLEWING_TRACKING) disp[2] = 'T';
  }
  else if (isSlewingTRK()) {
    disp[2] = 'T';
  }
  #if AZIMUTH_ALTITUDE_MOTORS == 1
  if (_stepperAZ->isRunning()) disp[3] = _stepperAZ->speed() < 0 ? 'Z' : 'z';
  if (_stepperALT->isRunning()) disp[4] = _stepperALT->speed() < 0 ? 'A' : 'a';
  #endif

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
  return _mountStatus & (STATUS_PARKING | STATUS_PARKING_POS);
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
      #if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
      _driverRA->microsteps(SET_MICROSTEPPING);
      //_driverRA->en_spreadCycle(1);  //only used as audiofeedback for quick debug
      //hier
      #endif
      if (direction & NORTH) {
        _stepperDEC->moveTo(sign * 300000);
        _mountStatus |= STATUS_SLEWING;
      }
      if (direction & SOUTH) {
        _stepperDEC->moveTo(-sign * 300000);
        _mountStatus |= STATUS_SLEWING;
      }
      if (direction & EAST) {
        _stepperRA->moveTo(-sign * 300000);
        _mountStatus |= STATUS_SLEWING;
      }
      if (direction & WEST) {
        _stepperRA->moveTo(sign * 300000);
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
    _stepperTRK->runSpeed();    
      if (_mountStatus & STATUS_GUIDE_PULSE_DEC) {
      _stepperDEC->runSpeed();
      }
      return;
    }

  if (_mountStatus & STATUS_TRACKING ) {
    //if ~(_mountStatus & STATUS_SLEWING) {
      _stepperTRK->runSpeed();
    //}
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

  #if AZIMUTH_ALTITUDE_MOTORS == 1
  _stepperAZ->run();
  _stepperALT->run();
  #endif
  
}

/////////////////////////////////
//
// loop
//
// Process any stepper changes. 
/////////////////////////////////
void Mount::loop() {
  unsigned long now = millis();
  bool raStillRunning = false;
  bool decStillRunning = false;
  
  // Since some of the boards cannot process timer interrupts at the required 
  // speed (or at all), we'll just stick to deterministic calls here.
  #if RUN_STEPPERS_IN_MAIN_LOOP == 1
  interruptLoop();
  #endif

  #if DEBUG_LEVEL&DEBUG_MOUNT 
  if (now - _lastMountPrint > 2000) {
    Serial.println(getStatusString());
    _lastMountPrint = now;
  }
  #endif

  #if AZIMUTH_ALTITUDE_MOTORS == 1
  if (!_stepperALT->isRunning() && !_stepperAZ->isRunning() && _azAltWasRunning)
  {
    // One of the motors was running last time through the loop, but not anymore, so shutdown the outputs.
    disableAzAltMotors();
    _azAltWasRunning = false;
  }
  if (_stepperALT->isRunning() || _stepperAZ->isRunning() )
  {
     _azAltWasRunning = true;
  }
  #endif

  #if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART && DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART && USE_AUTOHOME == 1
  if (isFindingHome()) {
    if (digitalRead(DEC_DIAG_PIN) == HIGH) {
      finishFindingHomeDEC();
      return;
    }
    if (digitalRead(RA_DIAG_PIN) == HIGH) {
      finishFindingHomeRA();
      return;
    }    
    //return;
  }
  #endif
  
  if (isGuiding()) {
    if (millis() > _guideEndTime) {
      stopGuiding();
	  #if DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
      _driverDEC->microsteps(DEC_SLEW_MICROSTEPPING);
    #endif					
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
        LOGV1(DEBUG_MOUNT,F("Mount::Loop: Reached target."));
        // Mount is at Target!
        // If we we're parking, we just reached home. Clear the flag, reset the motors and stop tracking.
        if (isParking()) {
          stopSlewing(TRACKING);
          // If we're on the second part of the slew to parking, don't set home here
          if (!_slewingToPark) {
            LOGV1(DEBUG_MOUNT,F("Mount::Loop:   Was Parking, stop tracking and set home."));
            setHome(false);
          }
          else{
            LOGV1(DEBUG_MOUNT,F("Mount::Loop:   Was Parking, stop tracking."));
          }
        }

        _currentDECStepperPosition = _stepperDEC->currentPosition();
        _currentRAStepperPosition = _stepperRA->currentPosition();
        #if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART
        _driverRA->microsteps(TRACKING_MICROSTEPPING);
        if (!isParking()) {
		      startSlewing(TRACKING);					   
        }
        //_driverRA->en_spreadCycle(0); // only for audio feedback for quick debug
        #endif
        if (_correctForBacklash) {
          LOGV3(DEBUG_MOUNT,F("Mount::Loop:   Reached target at %d. Compensating by %d"), (int)_currentRAStepperPosition, _backlashCorrectionSteps);
          _currentRAStepperPosition += _backlashCorrectionSteps;
          _stepperRA->runToNewPosition(_currentRAStepperPosition);
          _correctForBacklash = false;
          LOGV2(DEBUG_MOUNT,F("Mount::Loop:   Backlash correction done. Pos: %d"), _currentRAStepperPosition);
        }
        else
        {
          LOGV2(DEBUG_MOUNT,F("Mount::Loop:   Reached target at %d, no backlash compensation needed"), _currentRAStepperPosition);
        }

        if (_slewingToHome) {
          LOGV1(DEBUG_MOUNT,F("Mount::Loop:   Was Slewing home, so setting stepper RA and TRK to zero."));
          _stepperRA->setCurrentPosition(0);
          _stepperTRK->setCurrentPosition(0);
          _targetRA = currentRA();
          if (isParking()) {
            LOGV1(DEBUG_MOUNT,F("Mount::Loop:   Was parking, so no tracking. Proceeding to park position..."));
            _mountStatus &= ~STATUS_PARKING;
            _slewingToPark = true;
            _stepperRA->moveTo(_raParkingPos);
            _stepperDEC->moveTo(_decParkingPos);
            _totalDECMove = 1.0f * _stepperDEC->distanceToGo();
            _totalRAMove = 1.0f * _stepperRA->distanceToGo();
            LOGV5(DEBUG_MOUNT,F("Mount::Loop:   Park Position is R:%l  D:%l, TotalMove is R:%f, D:%f"), _raParkingPos,_decParkingPos,_totalRAMove, _totalDECMove);
            if ((_stepperDEC->distanceToGo() != 0) || (_stepperRA->distanceToGo() != 0)) {
              _mountStatus |= STATUS_PARKING_POS | STATUS_SLEWING;
            }
          }
          else {
            LOGV1(DEBUG_MOUNT,F("Mount::Loop:   Restart tracking."));
            startSlewing(TRACKING);
          }
          _slewingToHome = false;
        }
        else if (_slewingToPark)
        {
            LOGV1(DEBUG_MOUNT,F("Mount::Loop:   Arrived at park position..."));
            _mountStatus &= ~(STATUS_PARKING_POS | STATUS_SLEWING_TO_TARGET);
            _slewingToPark = false;
        }
        _totalDECMove = _totalRAMove = 0;

        // Make sure we do one last update when the steppers have stopped.
        displayStepperPosition();
        if (!inSerialControl) {
          _lcdMenu->updateDisplay();
        }
      }
    }

    if ((_bootComplete) && (now - _lastTrackingPrint > 200)) {
      _lcdMenu->printAt(15,0, isSlewingTRK() ? '&' : '`');
      _lastTrackingPrint = now;
    }

  }

  _stepperWasRunning = raStillRunning || decStillRunning;
}

/////////////////////////////////
//
// bootComplete
//
/////////////////////////////////
void Mount::bootComplete() {
    _bootComplete = true;
}

/////////////////////////////////
//
// setParkingPosition
//
/////////////////////////////////
void Mount::setParkingPosition(){
  _raParkingPos = _stepperRA->currentPosition() - _stepperTRK->currentPosition();
  _decParkingPos = _stepperDEC->currentPosition();

  LOGV3(DEBUG_MOUNT,F("Mount::setParkingPos: parking RA: %l  DEC:%l"), _raParkingPos, _decParkingPos);

  writePersistentData(EEPROM_RA_PARKING_POS, _raParkingPos);
  writePersistentData(EEPROM_DEC_PARKING_POS, _decParkingPos);
}

/////////////////////////////////
//
// setHome
//
/////////////////////////////////
void Mount::setHome(bool clearZeroPos) {
  LOGV1(DEBUG_MOUNT,F("Mount::setHome() called"));
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setHomePre: currentRA is %s"), currentRA().ToString());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setHomePre: targetRA is %s"), targetRA().ToString());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setHomePre: zeroPos is %s"), _zeroPosRA.ToString());
  _zeroPosRA = clearZeroPos ? DayTime(POLARIS_RA_HOUR, POLARIS_RA_MINUTE, POLARIS_RA_SECOND) :currentRA();

  _stepperRA->setCurrentPosition(0);
  _stepperDEC->setCurrentPosition(0);
  _stepperTRK->setCurrentPosition(0);

  _targetRA = currentRA();

  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setHomePost: currentRA is %s"), currentRA().ToString());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setHomePost: zeroPos is %s"), _zeroPosRA.ToString());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setHomePost: targetRA is %s"), targetRA().ToString());
}

/////////////////////////////////
//
// setTargetToHome
//
// Set RA and DEC to the home position
/////////////////////////////////
void Mount::setTargetToHome() {
  
  float trackedSeconds = _stepperTRK->currentPosition() / _trackingSpeed; // steps/steps/s
  
  LOGV2(DEBUG_MOUNT,F("Mount::setTargetToHome() called with %fs elapsed tracking"), trackedSeconds);

  // In order for RA coordinates to work correctly, we need to
  // offset HATime by elapsed time since last HA set and also
  // adjust RA by the elapsed time and set it to zero.
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setTargetToHomePre:  currentRA is %s"), currentRA().ToString());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setTargetToHomePre:  ZeroPosRA is %s"), _zeroPosRA.ToString());
  //LOGV3(DEBUG_MOUNT_VERBOSE,F("Mount::setTargetToHomePre:  TrackedSeconds is %f, TRK Stepper: %l"), trackedSeconds, _stepperTRK->currentPosition());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setTargetToHomePre:  LST is %s"), _LST.ToString());
  DayTime lst(_LST);
  lst.addSeconds(trackedSeconds);
  setLST(lst);
  _targetRA = _zeroPosRA;
  _targetRA.addSeconds(trackedSeconds);

  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setTargetToHomePost:  currentRA is %s"), currentRA().ToString());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setTargetToHomePost: ZeroPosRA is %s"), _zeroPosRA.ToString());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setTargetToHomePost: LST is %s"), _LST.ToString());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::setTargetToHomePost: TargetRA is %s"), _targetRA.ToString());

  // Set DEC to pole
  _targetDEC.set(0, 0, 0);
  _slewingToHome = true;
  // Stop the tracking stepper 
  LOGV1(DEBUG_MOUNT,F("Mount::setTargetToHome() stop tracking"));
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
  //LOGV3(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersPre: Current: RA: %s, DEC: %s"), currentRA().ToString(), currentDEC().ToString());
  //LOGV3(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersPre: Target : RA: %s, DEC: %s"), _targetRA.ToString(), _targetDEC.ToString());
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersPre: ZeroRA : %s"), _zeroPosRA.ToString());
  //LOGV4(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersPre: Stepper: RA: %l, DEC: %l, TRK: %l"), _stepperRA->currentPosition(), _stepperDEC->currentPosition(), _stepperTRK->currentPosition());
  DayTime raTarget = _targetRA;

  raTarget.subtractTime(_zeroPosRA);
  //LOGV3(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersIn: Adjust RA by Zeropos. New Target RA: %s, DEC: %s"), raTarget.ToString(), _targetDEC.ToString());

  float hourPos = raTarget.getTotalHours();
  if (!NORTHERN_HEMISPHERE) {
    hourPos += 12;
  }
  // Map [0 to 24] range to [-12 to +12] range
  while (hourPos > 12) {
    hourPos = hourPos - 24;
    //LOGV3(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersIn: RA>12 so -24. New Target RA: %s, DEC: %s"), DayTime(hourPos).ToString(), _targetDEC.ToString());
  }

  // How many steps moves the RA ring one sidereal hour along. One sidereal hour moves just shy of 15 degrees
  float stepsPerSiderealHour = _stepsPerRADegree * siderealDegreesInHour;

  // Where do we want to move RA to?
  #if RA_DRIVER_TYPE == DRIVER_TYPE_ULN2003
  float moveRA = hourPos * stepsPerSiderealHour / 2;
  #else
  float moveRA = hourPos * stepsPerSiderealHour;
  #endif



  // Where do we want to move DEC to?
  // the variable targetDEC 0deg for the celestial pole (90deg), and goes negative only.
  float moveDEC = -_targetDEC.getTotalDegrees() * _stepsPerDECDegree;

  //LOGV3(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersIn: RA Steps/deg: %d   Steps/srhour: %f"), _stepsPerRADegree, stepsPerSiderealHour);
  //LOGV3(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersIn: Target Step pos RA: %f, DEC: %f"), moveRA, moveDEC);

  // We can move 6 hours in either direction. Outside of that we need to flip directions.
#if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
  float RALimit = (6.0f * stepsPerSiderealHour / 2);
#else
  float RALimit = (6.0f * stepsPerSiderealHour);
#endif

  // If we reach the limit in the positive direction ...
  if (moveRA > RALimit) {
    //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersIn: RA is past +limit: %f, DEC: %f"), RALimit);

    // ... turn both RA and DEC axis around
#if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
    moveRA -= long(12.0f * stepsPerSiderealHour / 2);
#else
    moveRA -= long(12.0f * stepsPerSiderealHour);
#endif
    moveDEC = -moveDEC;
    //LOGV3(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersIn: Adjusted Target Step pos RA: %f, DEC: %f"), moveRA, moveDEC);
  }
  // If we reach the limit in the negative direction...
  else if (moveRA < -RALimit) {
    //LOGV2(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersIn: RA is past -limit: %f, DEC: %f"), -RALimit);
    // ... turn both RA and DEC axis around
#if RA_STEPPER_TYPE == STEPPER_TYPE_28BYJ48
    moveRA += long(12.0f * stepsPerSiderealHour / 2);
#else
    moveRA += long(12.0f * stepsPerSiderealHour);
#endif
    moveDEC = -moveDEC;
    //LOGV1(DEBUG_MOUNT_VERBOSE,F("Mount::CalcSteppersPost: Adjusted Target. Moved RA, inverted DEC"));
  }

  LOGV3(DEBUG_MOUNT,F("Mount::CalcSteppersPost: Target Steps RA: %f, DEC: %f"), -moveRA, moveDEC);
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

/////////////////////////////////
//
// moveSteppersTo
//
/////////////////////////////////
void Mount::moveSteppersTo(float targetRA, float targetDEC) {
  // Show time: tell the steppers where to go!
  _correctForBacklash = false;
  LOGV3(DEBUG_MOUNT,F("Mount::MoveSteppersTo: RA  From: %l  To: %f"), _stepperRA->currentPosition(), targetRA);
  LOGV3(DEBUG_MOUNT,F("Mount::MoveSteppersTo: DEC From: %l  To: %f"), _stepperDEC->currentPosition(), targetDEC);

  if ((_stepperRA->currentPosition() - targetRA) > 0) {
    LOGV2(DEBUG_MOUNT,F("Mount::MoveSteppersTo: Needs backlash correction of %d!"), _backlashCorrectionSteps);
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
    // Both axes moving to target
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

  if (abs(_totalDECMove) > 0.001) {
    // Only DEC moving to target
    float decDist = 100.0 - 100.0 * _stepperDEC->distanceToGo() / _totalDECMove;
    sprintf(scratchBuffer, "D%s %d%%", DECString(LCD_STRING | CURRENT_STRING).c_str(), (int)decDist);
    _lcdMenu->setCursor(0, 1);
    _lcdMenu->printMenu(String(scratchBuffer));
  }
  else if (abs(_totalRAMove) > 0.001) {
    // Only RAmoving to target
    float raDist = 100.0 - 100.0 * _stepperRA->distanceToGo() / _totalRAMove;
    sprintf(scratchBuffer, "R %s %d%%", RAString(LCD_STRING | CURRENT_STRING).c_str(), (int)raDist);
    disp = disp + String(scratchBuffer);
    _lcdMenu->setCursor(0, inSerialControl ? 0 : 1);
    _lcdMenu->printMenu(String(scratchBuffer));
  }
  else {
    // Nothing moving
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
    //LOGV1(DEBUG_MOUNT_VERBOSE,F("DECString: TARGET!"));
    dec = _targetDEC;
  }
  else {
    //LOGV1(DEBUG_MOUNT_VERBOSE,F("DECString: CURRENT!"));
    dec = DegreeTime(currentDEC());
  }
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("DECString: Precheck  : %s"), dec.ToString());
  dec.checkHours();
  //LOGV2(DEBUG_MOUNT_VERBOSE,F("DECString: Postcheck : %s"), dec.ToString());

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
#if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART && DEC_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART && USE_AUTOHOME == 1

void Mount::startFindingHomeDEC()  {
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

  delay(100);
  
  startFindingHomeRA(); 
}

void Mount::startFindingHomeRA()  {
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
   setHome(true);

}
#endif
