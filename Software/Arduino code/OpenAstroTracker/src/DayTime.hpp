#pragma once

#include <Arduino.h>
#include "inc/Config.hpp"

// A class to handle hours, minutes, seconds in a unified manner, allowing
// addition of hours, minutes, seconds, other times and conversion to string.

// DayTime handles a 24-hour time.
class DayTime
{
protected:
  long totalSeconds;
  long hourWrap = 24;

public:
  DayTime();

  DayTime(const DayTime &other);
  DayTime(int h, int m, int s);

  // From hours
  DayTime(float timeInHours);

  int getHours() const;
  int getMinutes() const;
  int getSeconds() const;
  float getTotalHours() const;
  float getTotalMinutes() const;
  long getTotalSeconds() const;

  void getTime(int &h, int &m, int &s) const;
  virtual void set(int h, int m, int s);
  void set(const DayTime &other);

  // Add hours, wrapping days (which are not tracked). Negative or positive.
  virtual void addHours(int deltaHours);

  // Add minutes, wrapping hours if needed
  void addMinutes(int deltaMins);

  // Add seconds, wrapping minutes and hours if needed
  void addSeconds(long deltaSecs);

  // Add time components, wrapping seconds, minutes and hours if needed
  void addTime(int deltaHours, int deltaMinutes, int deltaSeconds);

  // Add another time, wrapping seconds, minutes and hours if needed
  void addTime(const DayTime &other);
  // Subtract another time, wrapping seconds, minutes and hours if needed

  void subtractTime(const DayTime &other);

  // Convert to a standard string (like 14:45:06)
  virtual const char *ToString() const;
  virtual const char *formatString(char *targetBuffer, const char *format, long *pSeconds = nullptr) const;

  //protected:
  virtual void checkHours();

  static DayTime ParseFromMeade(String s);

protected:
  const char *formatStringImpl(char *targetBuffer, const char *format, char sgn, long degs, long mins, long secs) const;
  void printTwoDigits(char *achDegs, int num) const;
};
