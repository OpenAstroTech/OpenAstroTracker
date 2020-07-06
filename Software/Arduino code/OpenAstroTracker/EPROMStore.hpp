#pragma once
#include <Arduino.h>

// Platform independant abstraction of the EEPROM storage capability of the boards.
// This is needed because the ESP boards require two things that the Arduino boards don't:
//  1) It wants to know how many bytes you want to use (at most)
//  2) It wants you to call a commit() function after a write() to actual persist the data.
class EPROMStore {
  static EPROMStore *_eepromStore;
public:
  EPROMStore();
  static void initialize();

  void update(int location, uint8_t value);
  uint8_t read(int location);
  static EPROMStore* Storage();
};

