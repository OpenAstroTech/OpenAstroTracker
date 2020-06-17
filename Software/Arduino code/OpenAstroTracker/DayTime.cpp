#include "Utility.hpp"
#include "DayTime.hpp"

///////////////////////////////////
// DayTime (and DegreeTime below)
//
// A class to handle hours, minutes, seconds in a unified manner, allowing
// addition of hours, minutes, seconds, other times and conversion to string.

DayTime::DayTime() {
  hours = 0;
  mins = 0;
  secs = 0;
}

DayTime::DayTime(const DayTime& other) {
  hours = other.hours;
  mins = other.mins;
  secs = other.secs;
}

DayTime::DayTime(int h, int m, int s) {
  hours = h;
  mins = m;
  secs = s;
}

// From milliseconds. Does not handle days!
DayTime::DayTime(long ms) {
  ms /= 1000; // seconds
  secs = (int)(ms % 60);
  ms = (ms - secs) / 60;
  mins = (int)(ms % 60);
  ms = (ms - mins) / 60;
  hours = (int)ms;
}

DayTime::DayTime(float timeInHours) {
  hours = floor(timeInHours);
  timeInHours = (timeInHours - hours) * 60;
  mins = floor(timeInHours);
  timeInHours = (timeInHours - mins) * 60;
  secs = floor(timeInHours);
}

int DayTime::getHours() const {
  return hours;
}

int DayTime::getMinutes() const {
  return mins;
}

int DayTime::getSeconds() const {
  return secs;
}

float DayTime::getTotalHours() const {
  return 1.0f * getHours() + ((float)getMinutes() / 60.0f) + ((float)getSeconds() / 3600.0f);
}

float DayTime::getTotalMinutes() const {
  return 60.0f * getHours() + (float)getMinutes() + ((float)getSeconds() / 60.0f);
}

float DayTime::getTotalSeconds() const {
  return 3600.0f * getHours() + (float)getMinutes() * 60.0f + (float)getSeconds();
}

void DayTime::getTime(int& h, int& m, int& s) const {
  h = hours;
  m = mins;
  s = secs;
}

void DayTime::set(int h, int m, int s) {
  hours = h;
  mins = m;
  secs = s;
  checkHours();
}

void DayTime::set(const DayTime& other) {
  hours = other.hours;
  mins = other.mins;
  secs = other.secs;
  checkHours();
}

// Add hours, wrapping days (which are not tracked)
void DayTime::addHours(int deltaHours) {
  hours += deltaHours;
  checkHours();
}

void DayTime::checkHours() {
  while (hours >= hourWrap) {
    hours -= hourWrap;
  }

  while (hours < 0) {
    hours += hourWrap;
  }
}

// Add minutes, wrapping hours if needed
void DayTime::addMinutes(int deltaMins) {
  mins += deltaMins;
  while (mins > 59) {
    mins -= 60;
    addHours(1);
  }

  while (mins < 0) {
    mins += 60;
    addHours(-1);
  }
}

// Add seconds, wrapping minutes and hours if needed
void DayTime::addSeconds(long deltaSecs) {
  secs += deltaSecs;
  while (secs > 59) {
    secs -= 60;
    addMinutes(1);
  }

  while (secs < 0) {
    secs += 60;
    addMinutes(-1);
  }
}

// Add time components, wrapping seconds, minutes and hours if needed
void DayTime::addTime(int deltaHours, int deltaMinutes, int deltaSeconds)
{
  addSeconds(deltaSeconds);
  addMinutes(deltaMinutes);
  addHours(deltaHours);
}

// Add another time, wrapping seconds, minutes and hours if needed
void DayTime::addTime(const DayTime& other)
{
  addSeconds(other.getSeconds());
  addMinutes(other.getMinutes());
  addHours(other.getHours());
}

// Subtract another time, wrapping seconds, minutes and hours if needed
void DayTime::subtractTime(const DayTime& other)
{
  addSeconds(-other.getSeconds());
  addMinutes(-other.getMinutes());
  addHours(-other.getHours());
}

char achBuf[32];

// Convert to a standard string (like 14:45:06)
const char* DayTime::ToString() const
{
  char* p = achBuf;

  int absHours = hours < 0 ? -hours : hours;
  if (hours < 0)
  {
    *p++ = '-';
  }
  if (absHours < 10) {
    *p++ = '0';
  }
  else {
    *p++ = '0' + (absHours / 10);
  }

  *p++ = '0' + (absHours % 10);

  *p++ = ':';
  if (mins < 10) {
    *p++ = '0';
  }
  else {
    *p++ = '0' + (mins / 10);
  }

  *p++ = '0' + (mins % 10);
  *p++ = ':';
  if (secs < 10) {
    *p++ = '0';
  }
  else {
    *p++ = '0' + (secs / 10);
  }

  *p++ = '0' + (secs % 10);
  *p++ = ' ';
  *p++ = '(';
  strcpy(p, String(this->getTotalHours(), 5).c_str());
  strcat(p, ")");
  return achBuf;
}


DegreeTime::DegreeTime() : DayTime() {
  hourWrap = 180;
}

DegreeTime::DegreeTime(const DegreeTime& other) : DayTime(other) { }
DegreeTime::DegreeTime(int h, int m, int s) : DayTime(h, m, s) { }
DegreeTime::DegreeTime(float inDegrees) : DayTime(inDegrees) { }

void DegreeTime::addDegrees(int deltaDegrees) {
  addHours(deltaDegrees);
}

int DegreeTime::getDegrees() {
  return hours;
}

int DegreeTime::getPrintDegrees() const {
  return NORTHERN_HEMISPHERE ? hours + 90 : -90 - hours;
}

float DegreeTime::getTotalDegrees() const {
  return getTotalHours();
}

void DegreeTime::checkHours() {
  if (hours > 0) {
#ifdef DEBUG_MODE
    logv("CheckHours: Degrees is more than 0, clamping");
#endif
    hours = 0;
  }
  if (hours < -180) {
#ifdef DEBUG_MODE
    logv("CheckHours: Degrees is less than -180, clamping");
#endif
      hours = -180;
  }
}

char achBufDeg[32];

// Convert to a standard string (like 14:45:06)
const char* DegreeTime::ToString() const
{
  char* p = achBufDeg;
  int abshours = getPrintDegrees() < 0 ? -getPrintDegrees() : getPrintDegrees();

  if (getPrintDegrees() < 0) {
    *p++ = '-';
  }
  else {
    *p++ = '+';
  }

  if (abshours < 10) {
    *p++ = '0';
  }
  else {
    *p++ = '0' + (abshours / 10);
  }
  *p++ = '0' + (abshours % 10);

  *p++ = ':';
  if (mins < 10) {
    *p++ = '0';
  }
  else {
    *p++ = '0' + (mins / 10);
  }
  *p++ = '0' + (mins % 10);

  *p++ = ':';
  if (secs < 10) {
    *p++ = '0';
  }
  else {
    *p++ = '0' + (secs / 10);
  }

  *p++ = '0' + (secs % 10);
  *p++ = ' ';
  *p++ = '(';
  strcpy(p, String(NORTHERN_HEMISPHERE ? getTotalHours() + 90 : -90 - getTotalHours(), 4).c_str());
  strcat(p, ")");

  return achBufDeg;
}
