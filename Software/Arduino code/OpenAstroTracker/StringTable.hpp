#pragma once

// Strings used in the UI
#define UI_PAUSE_15SEC         0
#define UI_EASTWARD_PASS       1
#define UI_WESTWARD_PASS       2
#define UI_SLEWING             3
#define UI_RESET_MOUNT         4
#define UI_OAT                 5
#define UI_ADJUST_MOUNT        6
#define UI_CENTERED            7
#define UI_POLAR_ALIGNMENT     8  
#define UI_SPEED_CALIBRATION   9 
#define UI_RA_STEP_ADJUST     10
#define UI_DEC_STEP_ADJUST    11
#define UI_BACKLASH_ADJUST    12
#define UI_ROLL_OFFSET        13 
#define UI_PITCH_OFFSET       14
#define UI_MANUAL_CONTROL     15
#define UI_ROLL_DISPLAY       16
#define UI_PITCH_DISPLAY      17
#define UI_SPEED_FACTOR       18
#define UI_RA_STEPS           19
#define UI_DEC_STEPS          20
#define UI_BACKLASH           21

// Other strings
#define SLEW_TO_POLARIS  100
#define DRIFT_ALIGNMENT  101
#define RA_DEC_CONTROL   102
#define CONFIRM_HOME     103
#define DLG_SLEW_DISPLAY 104
#define STORE_AND_SYNC   105

// Function to retrieve the string
String oatString(int index);