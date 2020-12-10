#pragma once

#include <Arduino.h>
#include "DayTime.hpp"

// -180..180 range, 0 is at the prime meridian (through Greenwich), negative going west, positive going east
class Longitude : public DayTime
{
public:
  Longitude() : DayTime() {}
  Longitude(const Longitude &other);
  Longitude(int h, int m, int s);
  Longitude(float inDegrees);

  virtual const char *formatString(char *targetBuffer, const char *format, long *pSeconds = nullptr) const;

  static Longitude ParseFromMeade(String s);

protected:
  virtual void checkHours() override;
};

