#include <Arduino.h>
#include "Strings.hpp"

const char *stringConstants[] = {
    "Pause 1.5s...",
    "Eastward pass...",
    "Westward pass...",
    "Slewing...",
    "Reset mount...",

    "SlewToPolaris",
    "Drift Alignmnt",
};

// void initStringTable()
// {
//     stringConstants[UI_PAUSE_15SEC] = F("Pause 1.5s...");
//     stringConstants[UI_EASTWARD_PASS] = F("Eastward pass...");
//     stringConstants[UI_WESTWARD_PASS] = F("Westward pass...");
//     stringConstants[UI_SLEWING] = F("Slewing...");
//     stringConstants[UI_RESET_MOUNT] = F("Reset mount...");

//     stringConstants[SLEW_TO_POLARIS] = F("SlewToPolaris");
//     stringConstants[DRIFT_ALIGNMENT] = F("Drift Alignmnt");
// }

const char *oatString(int index)
{
    return stringConstants[index];
}
