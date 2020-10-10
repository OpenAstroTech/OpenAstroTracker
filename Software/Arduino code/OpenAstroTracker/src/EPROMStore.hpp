#pragma once
#include <Arduino.h>

// Platform independant abstraction of the EEPROM storage capability of the boards.
// This is needed because the ESP boards require two things that the Arduino boards don't:
//  1) It wants to know how many bytes you want to use (at most)
//  2) It wants you to call a commit() function after a write() to actual persist the data.
class EPROMStore {

public:

  static void initialize();

  static void update(int location, uint8_t value);
  static uint8_t read(int location);

  static void updateInt16(int loByteAddr, int hiByteAddr, int16_t value);
  static int16_t readInt16(int loByteAddr, int hiByteAddr);

  static void updateInt32(int lowestByteAddr, int32_t value);
  static int32_t readInt32(int lowestByteAddr);
};

