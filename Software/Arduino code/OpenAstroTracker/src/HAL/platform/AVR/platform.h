#pragma once

// include currently configured board 
#if BOARD == BOARD_AVR_MEGA2560
    #include "board/mega2560/board.h"
#elif BOARD == BOARD_AVR_MKS_GEN_L_2
    #include "board/mks_gen_l_2/board.h"
#else
    #error Unsupported AVR board
#endif