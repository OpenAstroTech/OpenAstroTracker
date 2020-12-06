#pragma once

// include currently configured board 
#if BOARD == BOARD_ESP32_ESP32DEV
    #include "boards/esp32dev/board.h"
#else
    #error Unsupported ESP32 board
#endif

#define GPS_SUPPORTED 0

#define ALTAZ_SUPPORTED 0

#define GYRO_SUPPORTED 0

#define DISPLAY_SUPPORTED 0