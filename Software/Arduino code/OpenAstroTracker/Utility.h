#ifndef UTILITY_H_
#define UTILITY_H_

#include <Arduino.h>
#include "Globals.h"

// LCD shield buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


class LcdButtons {
  public:
    LcdButtons(byte pin) {
      _analogPin = pin;
      _lastKeyChange = 0;

      _newKey = -1;
      _lastNewKey = -2;

      _currentKey = -1;
      _lastKey = -2;
    }

    byte currentKey() {
      checkKey();
      return _newKey;
    }

    byte currentState() {
      checkKey();
      return _currentKey;
    }

    int currentAnalogState() {
      checkKey();
      return _analogKeyValue;
    }

    bool keyChanged(byte& newKey) {
      checkKey();
      if (_newKey != _lastNewKey) {
        newKey = _newKey;
        _lastNewKey = _newKey;
        return true;
      }
      return false;
    }

  private:
    void checkKey() {
      _analogKeyValue = analogRead(_analogPin);
      if (_analogKeyValue > 1000) _currentKey = btnNONE;
      else if (_analogKeyValue < 50)   _currentKey =  btnRIGHT;
      else if (_analogKeyValue < 240)  _currentKey =  btnUP;
      else if (_analogKeyValue < 400)  _currentKey =  btnDOWN;
      else if (_analogKeyValue < 600)  _currentKey =  btnLEFT;
      else if (_analogKeyValue < 920)  _currentKey =  btnSELECT;

      if (_currentKey != _lastKey) {
        _lastKey = _currentKey ;
        _lastKeyChange = millis();
      } else {
        // If the keys haven't changed in 5ms, commit the change to the new keys.
        if (millis() - _lastKeyChange > 5) {
          // Serial.println("KBD: New=" + String(_currentKey ));
          _newKey = _currentKey ;
        }
      }
    }

  private:

    unsigned long _lastKeyChange;
    byte _analogPin;
    int _analogKeyValue;
    byte _lastKey;
    byte _newKey;
    byte _lastNewKey ;
    byte _currentKey;
};


// Adjust the given number by the given adjustment, wrap around the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
int adjustWrap(int current, int adjustBy, int minVal, int maxVal);

// Adjust the given number by the given adjustment, clamping to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
int adjustClamp(int current, int adjustBy, int minVal, int maxVal);

// Clamp the given number to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
long clamp(long current, long minVal, long maxVal);

// Clamp the given number to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
float clamp(float current, float minVal, float maxVal);

// Read the LCD Shield's key state and return the button being pressed (btnUP, etc.).
//int read_LCD_buttons();

#endif
