/**
 * This file is used to load all the hardware and platform dependant configurations and helpers. These are e.g. platform
 * and board specific pin configurations, interrupts, etc. Also different addon abstractions will be loaded here so they 
 * can be used in the main code without the need to know the actual implementation.
 **/

#pragma once

#include "../inc/Config.hpp"

// load the platform
#if defined(__AVR__)
    #include "platform/AVR/platform.h"
#elif defined(ARDUINO_ARCH_ESP32)
    #include "platform/ESP32/platform.h"
#endif

// 
#include "driver/Driver.hpp"

#ifndef INTERRUPTS_ENABLED
    #define INTERRUPTS_ENABLED 0
#endif