#pragma once

#include <Arduino.h>

// LCD shield buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

class LcdButtons {
public:
  LcdButtons(byte pin);
  byte currentKey();
  byte currentState();
  int currentAnalogState();
  bool keyChanged(byte* pNewKey);

private:
  void checkKey();

  unsigned long _lastKeyChange;
  byte _analogPin;
  int _analogKeyValue;
  byte _lastKey;
  byte _newKey;
  byte _lastNewKey;
  byte _currentKey;
};

