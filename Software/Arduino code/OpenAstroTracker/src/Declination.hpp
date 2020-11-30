#pragma once

#include <Arduino.h>
#include "inc/Config.hpp"
#include "DayTime.hpp"

// Declination is used to store DEC coordinate.
// The range of time is from -180 degrees to 0 degrees.
// In the northern hemisphere, 0 is north pole, -180 is south pole
// In the southern hemisphere, 0 is south pole, -180 is north pole
class Declination : public DayTime
{
public:
  Declination();
  Declination(const Declination &other);
  Declination(int h, int m, int s);
  Declination(float inDegrees);

  virtual void set(int h, int m, int s);

  // Add degrees, clamp to -180...0
  void addDegrees(int deltaDegrees);

  // Get total degrees (-180..0)
  float getTotalDegrees() const;

  // Convert to a standard string (like +54:45:06)
  virtual const char *ToString() const override;
  virtual const char *formatString(char *targetBuffer, const char *format, long *pSeconds = nullptr) const;

  const char *ToDisplayString(char sep1, char sep2) const;

protected:
  virtual void checkHours() override;

public:
  static Declination ParseFromMeade(String s);
  static Declination FromSeconds(long seconds);
};

