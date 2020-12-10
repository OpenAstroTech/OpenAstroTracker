#include "Utility.hpp"
#include "DayTime.hpp"

///////////////////////////////////
// DayTime (and Declination below)
//
// A class to handle hours, minutes, seconds in a unified manner, allowing
// addition of hours, minutes, seconds, other times and conversion to string.

// Parses the RA or DEC from a string that has an optional sign, a two digit degree, a seperator, a two digit minute, a seperator and a two digit second.
// Does not correct for hemisphere (derived class Declination takes care of that)
// For example:   -45*32:11 or 23:44:22
DayTime DayTime::ParseFromMeade(String s)
{
  DayTime result;
  int i = 0;
  long sgn = 1;
  LOGV2(DEBUG_MEADE, F("DayTime: Parse Coord from [%s]"), s.c_str());
  // Check whether we have a sign. This should be able to parse RA and DEC strings (RA never has a sign, and DEC should always have one).
  if ((s[i] == '-') || (s[i] == '+'))
  {
    sgn = s[i] == '-' ? -1 : +1;
    i++;
  }

  // Degs can be 2 or 3 digits
  long degs = s[i++] - '0';
  LOGV3(DEBUG_MEADE, F("DayTime: 1st digit [%c] -> degs=%l"), s[i - 1], degs);
  degs = degs * 10 + s[i++] - '0';
  LOGV3(DEBUG_MEADE, F("DayTime: 2nd digit [%c] -> degs=%l"), s[i - 1], degs);

  // Third digit?
  if ((s[i] >= '0') && (s[i] <= '9'))
  {
    degs = degs * 10 + s[i++] - '0';
    LOGV3(DEBUG_MEADE, F("DayTime: 3rd digit [%c] -> degs=%d"), s[i - 1], degs);
  }
  i++; // Skip seperator

  int mins = s.substring(i, i + 2).toInt();
  LOGV3(DEBUG_MEADE, F("DayTime: Minutes are [%s] -> mins=%d"), s.substring(i, i + 2).c_str(), mins);
  int secs = 0;
  if (s.length() > i + 4)
  {
    secs = s.substring(i + 3, i + 5).toInt();
    LOGV3(DEBUG_MEADE, F("DayTime: Seconds are [%s] -> secs=%d"), s.substring(i + 3, i + 5).c_str(), secs);
  }
  else
  {
    LOGV3(DEBUG_MEADE, F("DayTime: No Seconds. slen %d is not > %d"), s.length(), i + 4);
  }
  // Get the signed total seconds specified....
  result.totalSeconds = sgn * (((degs * 60L + mins) * 60L) + secs);

  LOGV5(DEBUG_MEADE, F("DayTime: TotalSeconds are %l from %lh %dm %ds"), result.totalSeconds, degs, mins, secs);

  return result;
}

DayTime::DayTime()
{
  totalSeconds = 0;
}

DayTime::DayTime(const DayTime &other)
{
  totalSeconds = other.totalSeconds;
}

DayTime::DayTime(int h, int m, int s)
{
  LOGV4(DEBUG_INFO, "DayTime ctor(%d, %d, %d)", h, m, s);
  long sgn = sign(h);
  h = abs(h);
  totalSeconds = sgn * ((60L* h + m) * 60L + s);
  LOGV6(DEBUG_INFO, "DayTime ctor(%d, %d, %d) -> %l -> %s", h, m, s, totalSeconds, ToString());
}

DayTime::DayTime(float timeInHours)
{
  LOGV2(DEBUG_INFO, "DayTime ctor(%f)", timeInHours);
  long sgn = fsign(timeInHours);
  timeInHours = abs(timeInHours);
  totalSeconds = sgn * round(timeInHours * 60 * 60);
  LOGV4(DEBUG_INFO, "DayTime ctor(%f) -> %l -> %s", timeInHours, totalSeconds, ToString());
}

int DayTime::getHours() const
{
  int h, m, s;
  getTime(h, m, s);
  return h;
}

int DayTime::getMinutes() const
{
  int h, m, s;
  getTime(h, m, s);
  return m;
}

int DayTime::getSeconds() const
{
  int h, m, s;
  getTime(h, m, s);
  return s;
}

float DayTime::getTotalHours() const
{
  return 1.0f * totalSeconds / 3600.0f;
}

float DayTime::getTotalMinutes() const
{
  return 1.0f * totalSeconds / 60.0f;
}

long DayTime::getTotalSeconds() const
{
  return totalSeconds;
}

void DayTime::getTime(int &h, int &m, int &s) const
{
  long seconds = abs(totalSeconds);

  h = (int)(seconds / 3600L);
  seconds = seconds - (h * 3600L);
  m = (int)(seconds / 60L);
  s = (int)(seconds - (m * 60L));

  h *= sign(totalSeconds);
}

void DayTime::set(int h, int m, int s)
{
  DayTime dt(h, m, s);
  totalSeconds = dt.totalSeconds;
  checkHours();
}

void DayTime::set(const DayTime &other)
{
  totalSeconds = other.totalSeconds;
  checkHours();
}

// Add hours, wrapping days (which are not tracked)
void DayTime::addHours(int deltaHours)
{
  totalSeconds += (long)deltaHours * 3600L;
  checkHours();
}

void DayTime::checkHours()
{
  long limit = hourWrap * 3600L;
  while (totalSeconds >= limit)
  {
    totalSeconds -= limit;
  }

  while (totalSeconds < 0)
  {
    totalSeconds += limit;
  }
}

// Add minutes, wrapping hours if needed
void DayTime::addMinutes(int deltaMins)
{
  totalSeconds += deltaMins * 60;
  checkHours();
}

// Add seconds, wrapping minutes and hours if needed
void DayTime::addSeconds(long deltaSecs)
{
  totalSeconds += deltaSecs;
  checkHours();
}

// Add another time, wrapping seconds, minutes and hours if needed
void DayTime::addTime(const DayTime &other)
{
  totalSeconds += other.totalSeconds;
  checkHours();
}

// Subtract another time, wrapping seconds, minutes and hours if needed
void DayTime::subtractTime(const DayTime &other)
{
  totalSeconds -= other.totalSeconds;
  checkHours();
}

char achBuf[32];

// Convert to a standard string (like 14:45:06)
const char *DayTime::ToString() const
{
  char *p = achBuf;
  int hours, mins, secs;
  getTime(hours, mins, secs);

  int absHours = abs(hours);
  if (totalSeconds < 0)
  {
    *p++ = '-';
  }
  if (absHours < 10)
  {
    *p++ = '0';
  }
  else
  {
    *p++ = '0' + (absHours / 10);
  }

  *p++ = '0' + (absHours % 10);

  *p++ = ':';
  if (mins < 10)
  {
    *p++ = '0';
  }
  else
  {
    *p++ = '0' + (mins / 10);
  }

  *p++ = '0' + (mins % 10);
  *p++ = ':';
  if (secs < 10)
  {
    *p++ = '0';
  }
  else
  {
    *p++ = '0' + (secs / 10);
  }

  *p++ = '0' + (secs % 10);
  *p++ = ' ';
  *p++ = '(';
  strcpy(p, String(this->getTotalHours(), 5).c_str());
  strcat(p, ")");
  return achBuf;
}
void DayTime::printTwoDigits(char *achDegs, int num) const
{
  achDegs[0] = '0' + (num / 10);
  achDegs[1] = '0' + (num % 10);
  achDegs[2] = 0;
}

const char *DayTime::formatStringImpl(char *targetBuffer, const char *format, char sgn, long degs, long mins, long secs) const
{
  char achDegs[5];
  char achMins[3];
  char achSecs[3];
  const char *f = format;
  char *p = targetBuffer;

  achDegs[0] = sgn;
  int i = 1;

  if (degs >= 100)
  {
    achDegs[i++] = '0' + (degs / 100);
    degs /= 10;
  }

  printTwoDigits(achDegs + i, degs);
  printTwoDigits(achMins, mins);
  printTwoDigits(achSecs, secs);

  char macro;
  bool inMacro = false;
  while (*f)
  {
    switch (*f)
    {
    case '{':
    {
      inMacro = true;
    }
    break;
    case '}':
    {
      if (inMacro)
      {
        switch (macro)
        {
        case 'd':
        {
          strcpy(p, achDegs);
          p += strlen(achDegs);
        }
        break;
        case 'm':
        {
          strcpy(p, achMins);
          p += 2;
        }
        break;
        case 's':
        {
          strcpy(p, achSecs);
          p += 2;
        }
        break;
        }
        inMacro = false;
      }
    }
    break;
    default:
    {
      if (inMacro)
      {
        macro = *f;
      }
      else
      {
        *p++ = *f;
      }
    }
    }
    f++;
  }

  *p = '\0';
  return targetBuffer;
}

const char *DayTime::formatString(char *targetBuffer, const char *format, long *pSecs) const
{
  long secs = pSecs == nullptr ? totalSeconds : *pSecs;
  char sgn = secs < 0 ? '-' : '+';
  secs = abs(secs);
  long degs = secs / 3600;
  secs = secs - degs * 3600;
  long mins = secs / 60;
  secs = secs - mins * 60;

  return formatStringImpl(targetBuffer, format, sgn, degs, mins, secs);
}
