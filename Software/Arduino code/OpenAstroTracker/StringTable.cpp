#include <Arduino.h>
#include "StringTable.hpp"

String oatString(int index)
{
    switch (index) {
        case UI_PAUSE_15SEC: return F("Pause 1.5s...");
        case UI_EASTWARD_PASS :  return F("Eastward pass...");
        case UI_WESTWARD_PASS :  return F("Westward pass...");
        case UI_SLEWING :  return F("Slewing...");
        case UI_OAT :  return F("OpenAstroTracker");
        case UI_RESET_MOUNT :  return F("Reset mount...");
        case UI_ADJUST_MOUNT: return F("Adjust mount");
        case UI_CENTERED : return F("Centered");
        case UI_POLAR_ALIGNMENT : return F("Polar Alignmnt");
        case UI_SPEED_CALIBRATION: return F("Speed Calibratn");
        case UI_RA_STEP_ADJUST : return F("RA Step Adjust");
        case UI_DEC_STEP_ADJUST : return F("DEC Step Adjust");
        case UI_BACKLASH_ADJUST: return F("Backlash Adjust");
        case UI_ROLL_OFFSET: return F("Roll Offset");
        case UI_PITCH_OFFSET: return F("Pitch Offset");
        case UI_MANUAL_CONTROL : return F("Manual Control");
        case UI_ROLL_DISPLAY : return F("R: -------------");
        case UI_PITCH_DISPLAY : return F("P: -------------");
        case UI_SPEED_FACTOR : return F("SpdFctr: @");
        case UI_RA_STEPS  : return F("RA Steps: @");
        case UI_DEC_STEPS  : return F("DEC Steps: @");
        case UI_BACKLASH  : return F("Backlash: @");

        case SLEW_TO_POLARIS :  return F("SlewToPolaris");
        case DRIFT_ALIGNMENT :  return F("Drift Alignmnt");
        case RA_DEC_CONTROL : return F("RADECControl");
        case CONFIRM_HOME : return F("ConfirmHome");
        case STORE_AND_SYNC : return F("StoreAndSync");
        case DLG_SLEW_DISPLAY : return F("SlewDisplay");


        default: return F("Unknown!!!");
    }
}
