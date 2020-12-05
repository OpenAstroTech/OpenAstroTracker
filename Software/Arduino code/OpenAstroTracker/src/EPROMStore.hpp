#pragma once
#include <Arduino.h>

// Platform independant abstraction of the EEPROM storage capability of the boards.
// This is needed because the ESP boards require two things that the Arduino boards don't:
//  1) It wants to know how many bytes you want to use (at most)
//  2) It wants you to call a commit() function after a write() to actual persist the data.
class EPROMStore {

public:

  enum Key { 
    SPEED_FACTOR_LOW=0, SPEED_FACTOR_HIGH=3,  // Split as two discontinuous Uint8
    HA_HOUR=1, HA_MINUTE=2, // Both Uint8
    FLAGS=4,  // Uint8
    MAGIC_MARKER=5, // Uint8
    MAGIC_MARKER_AND_FLAGS=4,   // Alias for Uint16 access
    RA_STEPS_DEGREE=6, _RA_STEPS_DEGREE_1=7,    // Int16
    DEC_STEPS_DEGREE=8, _DEC_STEPS_DEGREE_1=9,  // Int16
    BACKLASH_STEPS=10, _BACKLASH_STEPS_1=11,    // Int16
    LATITUDE=12, _LATITUDE_1=13,    // Int16
    LONGITUDE=14, _LONGITUDE_1=13,  // Int16
    LCD_BRIGHTNESS=16, // Uint8
    PITCH_OFFSET=17, _PITCH_OFFSET_1=18,  // Uint16
    ROLL_OFFSET=19, _ROLL_OFFSET_1=20,    // Uint16
    EXTENDED_FLAGS=21, _EXTENDED_FLAGS_1=22,  // Uint16
    RA_PARKING_POS=23, _RA_PARKING_POS_1, _RA_PARKING_POS_2, _RA_PARKING_POS_3,     // Int32
    DEC_PARKING_POS=27, _DEC_PARKING_POS_1, _DEC_PARKING_POS_2, _DEC_PARKING_POS_3, // Int32
    DEC_LOWER_LIMIT=31, _DEC_LOWER_LIMIT_1, _DEC_LOWER_LIMIT_2, _DEC_LOWER_LIMIT_3, // Int32
    DEC_UPPER_LIMIT=35, _DEC_UPPER_LIMIT_1, _DEC_UPPER_LIMIT_2, _DEC_UPPER_LIMIT_3, // Int32
    STORE_SIZE=64   
  };

  static void initialize();

  static void updateUint8(Key location, uint8_t value);
  static uint8_t readUint8(Key location);

  static void updateUint16(Key location, uint16_t value);
  static uint16_t readUint16(Key location);

  static void updateInt16(Key location, int16_t value);
  static int16_t readInt16(Key location);

  static void updateInt32(Key location, int32_t value);
  static int32_t readInt32(Key location);

private:
  static void update(uint8_t location, uint8_t value);
  static uint8_t read(uint8_t location);
};

