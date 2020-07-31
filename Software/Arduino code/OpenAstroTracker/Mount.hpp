#ifndef _MOUNT_HPP_
#define _MOUNT_HPP_

#include <LiquidCrystal.h>
#include <AccelStepper.h>
#include "Globals.hpp"
#include "DayTime.hpp"
#include "LcdMenu.hpp"

#define NORTH                      B00000001
#define EAST                       B00000010
#define SOUTH                      B00000100
#define WEST                       B00001000
#define ALL_DIRECTIONS             B00001111
#define TRACKING                   B00010000

#define LCDMENU_STRING      B0001
#define MEADE_STRING        B0010
#define PRINT_STRING        B0011
#define LCD_STRING          B0100
#define COMPACT_STRING      B0101
#define FORMAT_STRING_MASK  B0111

#define TARGET_STRING      B01000
#define CURRENT_STRING     B10000

#define HALFSTEP 8
#define FULLSTEP 4
#define DRIVER 1

#define RA_STEPS  1
#define DEC_STEPS 2
#define SPEED_FACTOR_DECIMALS 3
#define BACKLASH_CORRECTION 4

//////////////////////////////////////////////////////////////////
//
// Class that represent the OpenAstroTracker mount, with all its parameters, motors, etc.
//
//////////////////////////////////////////////////////////////////
class Mount {
public:
  Mount(int stepsPerRADegree, int stepsPerDECDegree, LcdMenu* lcdMenu);

  // Configure the RA stepper motor. This also sets up the TRK stepper on the same pins.
#if RA_Stepper_TYPE == 0
    void configureRAStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration);
#endif
#if RA_Stepper_TYPE == 1
    void configureRAStepper(byte stepMode, byte pin1, byte pin2, int maxSpeed, int maxAcceleration);
#endif

  // Configure the DEC stepper motor.
#if DEC_Stepper_TYPE == 0
    void configureDECStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration);
#endif
#if DEC_Stepper_TYPE == 1
    void configureDECStepper(byte stepMode, byte pin1, byte pin2, int maxSpeed, int maxAcceleration);
#endif

  // Get the current RA tracking speed factor
  float getSpeedCalibration();

  // Set the current RA tracking speed factor
  void setSpeedCalibration(float val, bool saveToStorage);

  // Returns the number of steps the given motor turns to move one degree
  int getStepsPerDegree(int which);

  // Function to set the number of steps the given motor turns to move one 
  // degree for each axis. This function stores the value in persistent storage
  void setStepsPerDegree(int which, int steps);

  // Sets the slew rate of the mount. rate is between 1 (slowest) and 4 (fastest)
  void setSlewRate(int rate);

  // Set the HA time (HA is derived from LST, the setter calculates and sets LST)
  void setHA(const DayTime& haTime);
  const DayTime HA() const;

  // Set the LST time (HA is derived from LST)
  void setLST(const DayTime& haTime);
  const DayTime& LST() const;

  void setLatitude(float lat);
  void setLongitude(float lon);
  const float latitude() const;
  const float longitude() const;

  // Get a reference to the target RA value.
  DayTime& targetRA();

  // Get a reference to the target DEC value.
  DegreeTime& targetDEC();

  // Get current RA value.
  const DayTime currentRA() const;

  // Get current DEC value.
  const DegreeTime currentDEC() const;

  // Set the current RA and DEC position to be the given coordinates
  void syncPosition(int raHour, int raMinute, int raSecond, int decDegree, int decMinute, int decSecond);

  // Calculates movement parameters and program steppers to move
  // there. Must call loop() frequently to actually move.
  void startSlewingToTarget();

  // Various status query functions
  bool isSlewingDEC() const;
  bool isSlewingRA() const;
  bool isSlewingRAorDEC() const;
  bool isSlewingIdle() const;
  bool isSlewingTRK() const;
  bool isParked() const;
  bool isParking() const;
  bool isGuiding() const;

  // Starts manual slewing in one of eight directions or tracking
  void startSlewing(int direction);

  // Stop manual slewing in one of two directions or tracking. NS is the same. EW is the same
  void stopSlewing(int direction);

  // Block until the motors specified (NORTH, EAST, TRACKING, etc.) are stopped
  void waitUntilStopped(byte direction);

  // Same as Arduino delay() but keeps the tracker going.
  void delay(int ms);

  // Gets the position in one of eight directions or tracking
  long getCurrentStepperPosition(int direction);

  // Process any stepper movement. 
  void loop();

  // Low-leve process any stepper movement on interrupt callback.
  void interruptLoop();

  // Set RA and DEC to the home position
  void setTargetToHome();

  // Asynchronously slews the mount to the home position 
  void goHome();

  // Set the current stepper positions to be home.
  void setHome(bool clearZeroPos);

  // Asynchronously parks the mount. Moves to the home position and stops all motors. 
  void park();

  // Runs the RA motor at twice the speed (or stops it), or the DEC motor at tracking speed for the given duration in ms.
  void guidePulse(byte direction, int duration);

  // Stops any guide operation in progress.
  void stopGuiding();

  // Return a string of DEC in the given format. For LCDSTRING, active determines where the cursor is
  String DECString(byte type, byte active = 0);

  // Return a string of DEC in the given format. For LCDSTRING, active determines where the cursor is
  String RAString(byte type, byte active = 0);

  // Returns a comma-delimited string with all the mounts' information
  String getStatusString();

  // Get the current speed of the stepper. NORTH, WEST, TRACKING
  float getSpeed(int direction);

  // Displays the current location of the mount every n ms, where n is defined in Globals.h as DISPLAY_UPDATE_TIME
  void displayStepperPositionThrottled();

  // Runs a phase of the drift alignment procedure
  void runDriftAlignmentPhase(int direction, int durationSecs);

  // Toggle the state where we run the motors at a constant speed
  void setManualSlewMode(bool state);

  // Set the speed of the given motor
  void setSpeed(int which, float speed);

  // Set the number of steps to use for backlash correction
  void setBacklashCorrection(int steps);

  // Get the number of steps to use for backlash correction
  int getBacklashCorrection();
  
  // Called when startup is complete and the mount needs to start updating steppers.
  void  startTimerInterrupts();

  // Read the saved configuration from persistent storage
  void readConfiguration();
  
  // Get Mount configuration data
  String getMountHardwareInfo();

private:

  // Reads values from EEPROM that configure the mount (if previously stored)
  void readPersistentData();

  // Writes a 16-bit value to persistent (EEPROM) storage
  void writePersistentData(int which, int val);

  void calculateRAandDECSteppers(float& targetRA, float& targetDEC);
  void displayStepperPosition();
  void moveSteppersTo(float targetRA, float targetDEC);

  // Returns NOT_SLEWING, SLEWING_DEC, SLEWING_RA, or SLEWING_BOTH. SLEWING_TRACKING is an overlaid bit.
  byte slewStatus() const;

  // What is the state of the mount. 
  // Returns some combination of these flags: STATUS_PARKED, STATUS_SLEWING, STATUS_SLEWING_TO_TARGET, STATUS_SLEWING_FREE, STATUS_TRACKING, STATUS_PARKING
  byte mountStatus();

#if DEBUG_LEVEL&(DEBUG_MOUNT|DEBUG_MOUNT_VERBOSE)
  String mountStatusString();
#endif


private:
  LcdMenu* _lcdMenu;
  int  _stepsPerRADegree;
  int _stepsPerDECDegree;
  int _maxRASpeed;
  int _maxDECSpeed;
  int _maxRAAcceleration;
  int _maxDECAcceleration;
  int _backlashCorrectionSteps;
  int _moveRate;

  long _lastHASet;
  DayTime _LST;
  DayTime _zeroPosRA;

  DayTime _targetRA;
  long _currentRAStepperPosition;

  DegreeTime _targetDEC;
  long _currentDECStepperPosition;

  float _totalDECMove;
  float _totalRAMove;
  float _latitude;
  float _longitude;

  // Stepper control for RA, DEC and TRK.
  AccelStepper* _stepperRA;
  AccelStepper* _stepperDEC;
  AccelStepper* _stepperTRK;

  unsigned long _guideEndTime;
  unsigned long _lastMountPrint = 0;
  float _trackingSpeed;
  float _trackingSpeedCalibration;
  unsigned long _lastDisplayUpdate;
  volatile int _mountStatus;
  char scratchBuffer[24];
  bool _stepperWasRunning;
  bool _correctForBacklash;
  bool _slewingToHome;
};

#endif
