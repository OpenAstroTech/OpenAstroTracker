#pragma once
#include <Arduino.h>

#define DEBUG_ENABLED 1

String getLogBuffer();
int freeMemory();

#if DEBUG_ENABLED == 1

// String formatArg(const char* input, va_list args);
// String format(const char* input, ...);
// // void log(const char* input);
// // void log(String input);
void logv(int levelFlags, const char* input, ...);

#define LOGV1(level,a) logv((level),(a))
#define LOGV2(level,a,b) logv((level),(a),(b))
#define LOGV3(level,a,b,c) logv((level),(a),(b),(c))
#define LOGV4(level,a,b,c,d) logv((level),(a),(b),(c),(d))
#define LOGV5(level,a,b,c,d,e) logv((level),(a),(b),(c),(d),(e))
#define LOGV6(level,a,b,c,d,e,f) logv((level),(a),(b),(c),(d),(e),(f))
#define LOGV7(level,a,b,c,d,e,f,g) logv((level),(a),(b),(c),(d),(e),(f),(g))
#define LOGV8(level,a,b,c,d,e,f,g,h) logv((level),(a),(b),(c),(d),(e),(f),(g),(h))

// Realtime timer class using microseconds to time stuff
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

// Performance measurement class 
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



#else // DEBUG_ENABLED 


#define LOGV1(level,a) 
#define LOGV2(level,a,b) 
#define LOGV3(level,a,b,c) 
#define LOGV4(level,a,b,c,d) 
#define LOGV5(level,a,b,c,d,e) 
#define LOGV6(level,a,b,c,d,e,f) 
#define LOGV7(level,a,b,c,d,e,f,g)

#endif // not DEBUG_ENABLED

void setDebugLevel(int level);
