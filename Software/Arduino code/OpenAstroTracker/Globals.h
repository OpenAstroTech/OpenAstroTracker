#ifndef _GLOBALS_H_
#define _GLOBALS_H_
// Set to 1 if you are in the northern hemisphere.
#define NORTHERN_HEMISPHERE 1

// Time in ms between LCD screen updates during slewing operations
#define DISPLAY_UPDATE_TIME 100


// LCD shield buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


extern int lcd_key;         // The current key state
extern int adc_key_in;      // The analog value of the keys

#endif
