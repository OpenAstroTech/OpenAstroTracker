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

#ifdef DEBUG_MODE

class RealTime {
  static unsigned long _pausedTime;
  static unsigned long _startTime;
  static unsigned long _suspendStart;
  static int  _suspended;
public:
  static void suspend() {
    if (_suspended == 0) {
      _suspendStart = micros();
    }
    _suspended++;
  }

  static void resume() {
    _suspended--;
    if (_suspended == 0) {
      unsigned long now = micros();
      _pausedTime += now - _suspendStart;
    }
  }

  static unsigned long currentTime() {
    if (_suspended != 0) {
      unsigned long now = micros();
      unsigned long pausedUntilNow = now - _suspendStart;
      return now - pausedUntilNow;
    }
    else {
      return micros() - _pausedTime;
    }
  }
};

class PerfMeasure {
  unsigned long _start;
  unsigned long _end;
  unsigned long _duration;
  int _indent;
  String _name;
  bool _running;
  bool _printed;

public:
  PerfMeasure(int indent, String name) {
    _name = name;
    _running = true;
    _printed = false;
    _indent = indent;
    _start = RealTime::currentTime();
  }

  ~PerfMeasure() {
    if (_running) stop();
    print();
  }

  void stop() {
    _end = RealTime::currentTime();
    _duration = _end - _start;
    _running = false;
  }

  float durationMs() {
    return 0.001 * _duration;
  }

  void print() {
    if (_running) stop();
    RealTime::suspend();
    if (!_printed) {
      char buf[128];
      memset(buf, ' ', 127);
      buf[127] = 0;

      String disp = String(durationMs(), 3);
      char* p = buf + (_indent * 1);
      memcpy(p, _name.c_str(), _name.length());
      p = buf + 36 - disp.length();
      memcpy(p, disp.c_str(), disp.length());
      p = buf + 36;
      *p++ = 'm';
      *p++ = 's';
      *p++ = 0;
      Serial.println(String(buf));
      _printed = true;
    }
    RealTime::resume();
  }
};

String formatArg(const char* input, va_list args);
String format(const char* input, ...);
void log(const char* input);
void log(String input);
void logv(const char* input, ...);

#endif

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

  bool keyChanged(byte* pNewKey) {
    checkKey();
    if (_newKey != _lastNewKey) {
      *pNewKey = _newKey;
      _lastNewKey = _newKey;
      return true;
    }
    return false;
  }

private:
  void checkKey() {
    _analogKeyValue = analogRead(_analogPin);
    if (_analogKeyValue > 1000) _currentKey = btnNONE;
    else if (_analogKeyValue < 50)   _currentKey = btnRIGHT;
    else if (_analogKeyValue < 240)  _currentKey = btnUP;
    else if (_analogKeyValue < 400)  _currentKey = btnDOWN;
    else if (_analogKeyValue < 600)  _currentKey = btnLEFT;
    else if (_analogKeyValue < 920)  _currentKey = btnSELECT;

    if (_currentKey != _lastKey) {
      _lastKey = _currentKey;
      _lastKeyChange = millis();
    }
    else {
      // If the keys haven't changed in 5ms, commit the change to the new keys.
      if (millis() - _lastKeyChange > 5) {
        // Serial.println("KBD: New=" + String(_currentKey ));
        _newKey = _currentKey;
      }
    }
  }

private:

  unsigned long _lastKeyChange;
  byte _analogPin;
  int _analogKeyValue;
  byte _lastKey;
  byte _newKey;
  byte _lastNewKey;
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
