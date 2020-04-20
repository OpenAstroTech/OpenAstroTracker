#ifndef _MOUNT_HPP_
#define _MOUNT_HPP_

#include <LiquidCrystal.h>
#include <AccelStepper.h>
#include "Globals.h"
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
#define FORMAT_STRING_MASK  B0111

#define TARGET_STRING      B01000
#define CURRENT_STRING     B10000

#define HALFSTEP 8
#define FULLSTEP 4

//////////////////////////////////////////////////////////////////
//
// Class that represent the OpenAstroTracker mount, with all its parameters, motors, etc.
//
//////////////////////////////////////////////////////////////////
class Mount {
  public:
    Mount(int stepsPerRAHour, int stepsPerDECDegree, LcdMenu* lcdMenu);

    // Configure the RA stepper motor. This also sets up the TRK stepper on the same pins.
    void configureRAStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration);

    // Configure the DEC stepper motor.
    void configureDECStepper(byte stepMode, byte pin1, byte pin2, byte pin3, byte pin4, int maxSpeed, int maxAcceleration);

    // Set the HA time
    void setHA(const DayTime & haTime);
    const DayTime& HA() const;
    void setHACorrection(int h, int m, int s);
    DayTime getHACorrection();

    // Get a reference to the target RA value.
    DayTime& targetRA();

    // Get a reference to the target DEC value.
    DegreeTime& targetDEC();

    // Get current RA value.
    const DayTime currentRA() const;

    // Get current DEC value.
    const DegreeTime currentDEC() const;

    // Set the current RA position to be the given time
    void syncRA(int hour, int minute, int second);

    // Set the current DEC position to be the given degrees
    void syncDEC(int degree, int minute, int second);

    float getSpeedCalibration();

    void setSpeedCalibration(float val);

    // Calculates movement parameters and program steppers to move
    // there. Must call loop() frequently to actually move.
    void startSlewingToTarget();

    bool isSlewingDEC();
    bool isSlewingRA();
    bool isSlewingRAorDEC();
    bool isSlewingIdle();
    bool isSlewingTRK();
    bool isParked();
    bool isParking();
    bool isGuiding();

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

    // Process any stepper movement. Must be called frequently
    void loop();

    // Set RA and DEC to the home position
    void setTargetToHome();

    // Synchronously slews the mount to the home position and sets tracking to argument.
    void goHome(bool tracking);

    // Set the current stepper positions to be home.
    void setHome();

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

    // Get the current speed of the stepper. NORTH, WEST, TRACKING
    float getSpeed(int direction);

    void displayStepperPositionThrottled();

    void runDriftAlignmentPhase(int direction, int durationSecs);

  private:
    void calculateRAandDECSteppers(float& targetRA, float& targetDEC);
    void displayStepperPosition();
    void moveSteppersTo(float targetRA, float targetDEC);

    // Returns NOT_SLEWING, SLEWING_DEC, SLEWING_RA, or SLEWING_BOTH. SLEWING_TRACKING is an overlaid bit.
    byte slewStatus();

    // What is the state of the mount. 
    // Returns some combination of these flags: STATUS_PARKED, STATUS_SLEWING, STATUS_SLEWING_TO_TARGET, STATUS_SLEWING_FREE, STATUS_TRACKING, STATUS_PARKING
    byte mountStatus();
    
#ifdef DEBUG_MODE
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
    
    long _lastHASet;
    DayTime _HAAdjust;

    DayTime _targetRA;
    DayTime _currentRA;
    long _currentRAStepperPosition;

    DegreeTime _targetDEC;
    DegreeTime _currentDEC;
    long _currentDECStepperPosition;

    float _totalDECMove;
    float _totalRAMove;

    // Stepper control for RA, DEC and TRK.
    AccelStepper* _stepperRA;
    AccelStepper* _stepperDEC;
    AccelStepper* _stepperTRK;

    unsigned long _guideEndTime;
    unsigned long _lastMountPrint = 0;
    DayTime _HATime;
    DayTime _HACorrection;
    float _trackingSpeed;
    float _trackingSpeedCalibration;
    unsigned long _lastDisplayUpdate;
    byte _mountStatus;
    char scratchBuffer[24];
    bool _stepperWasRunning;
};

#endif
