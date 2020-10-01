#pragma once

#define UI_PAUSE_15SEC   0
#define UI_EASTWARD_PASS 1
#define UI_WESTWARD_PASS 2
#define UI_SLEWING       3
#define UI_RESET_MOUNT   4

#define SLEW_TO_POLARIS 5
#define DRIFT_ALIGNMENT 6

#define NUMBER_OF_STRINGS 7

//extern const char* stringConstants[];
const char *oatString(int index);