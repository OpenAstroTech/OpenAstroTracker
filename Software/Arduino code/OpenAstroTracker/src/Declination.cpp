#include "Utility.hpp"
#include "Declination.hpp"

//////////////////////////////////////////////////////////////////////////////////////
//
// Declination 
//
// A class to handle degrees, minutes, seconds of a declination

// Parses the RA or DEC from a string that has an optional sign, a two digit degree, a seperator, a two digit minute, a seperator and a two digit second.
// For example:   -45*32:11 or 23:44:22

// In the northern hemisphere, 0 is north pole, -180 is south pole
// In the southern hemisphere, 0 is south pole, -180 is north pole
Declination::Declination() : DayTime()
{
  hourWrap = 180L;
}

Declination::Declination(const Declination &other) : DayTime(other)
{
  hourWrap = 180L;
}

Declination::Declination(int h, int m, int s) : DayTime(h, m, s)
{
  hourWrap = 180L;
}

Declination::Declination(float inDegrees) : DayTime(inDegrees)
{
  hourWrap = 180L;
}

void Declination::set(int h, int m, int s)
{
  Declination dt(h, m, s);
  totalSeconds = dt.totalSeconds;
  checkHours();
}

void Declination::addDegrees(int deltaDegrees)
{
  addHours(deltaDegrees);
}

float Declination::getTotalDegrees() const
{
  return getTotalHours();
}

void Declination::checkHours()
{
  if (totalSeconds > 0)
  {
    LOGV1(DEBUG_GENERAL, F("CheckHours: Degrees is more than 0, clamping"));
    totalSeconds = 0;
  }
  if (totalSeconds < (-180L * 3600L))
  {
    LOGV1(DEBUG_GENERAL, F("CheckHours: Degrees is less than -180, clamping"));
    totalSeconds = -180L * 3600L;
  }
}

char achBufDeg[32];

// Convert to a standard string (like 14:45:06), specifying separators if needed
const char *Declination::ToDisplayString(char sep1, char sep2) const
{
  char achFormat[16];
  sprintf(achFormat, "{d}%c{m}%c{s}", sep1, sep2);
  return formatString(achBufDeg, achFormat);
}

const char *Declination::ToString() const
{
  ToDisplayString('*', ':');

  char *p = achBufDeg + strlen(achBufDeg);

  *p++ = ' ';
  *p++ = '(';
  strcpy(p, String(NORTHERN_HEMISPHERE ? getTotalHours() + 90 : -90 - getTotalHours(), 4).c_str());
  strcat(p, ")");

  return achBufDeg;
}

Declination Declination::ParseFromMeade(String s)
{
  Declination result;
  LOGV2(DEBUG_GENERAL, F("Declination.Parse(%s)"), s.c_str());

  // Use the DayTime code to parse it...
  DayTime dt = DayTime::ParseFromMeade(s);

  // ...and then correct for hemisphere
  result.totalSeconds = dt.getTotalSeconds() + (NORTHERN_HEMISPHERE ? -90L : 90L) * 3600L;
  LOGV3(DEBUG_GENERAL, F("Declination.Parse(%s) -> %s"), s.c_str(), result.ToString());
  return result;
}

Declination Declination::FromSeconds(long seconds)
{
  seconds += (NORTHERN_HEMISPHERE ? -90L * 3600L : 90L * 3600L);
  return Declination(1.0 * seconds / 3600.0);
}

const char *Declination::formatString(char *targetBuffer, const char *format, long *) const
{
  long secs = totalSeconds;
  secs = NORTHERN_HEMISPHERE ? secs + 90L * 3600L : -90L * 3600L - secs;
  return DayTime::formatString(targetBuffer, format, &secs);
}
