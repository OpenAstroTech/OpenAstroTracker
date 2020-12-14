#include "Utility.hpp"
#include "Longitude.hpp"

//////////////////////////////////////////////////////////////////////////////////////
//
// -180..180 range, 0 is at the prime meridian (through Greenwich), negative going west, positive going east

Longitude::Longitude(const Longitude &other) : DayTime(other)
{
}

Longitude::Longitude(int h, int m, int s) : DayTime(h, m, s)
{
}

Longitude::Longitude(float inDegrees) : DayTime(inDegrees)
{
}

void Longitude::checkHours()
{
  while (totalSeconds > 180L * 3600L)
  {
    LOGV1(DEBUG_GENERAL, F("LongitudeCheckHours: Degrees is more than 180, wrapping"));
    totalSeconds -= 360L * 3600L;
  }
  while (totalSeconds < (-180L * 3600L))
  {
    LOGV1(DEBUG_GENERAL, F("LongitudeCheckHours: Degrees is less than -180, wrapping"));
    totalSeconds += 360L * 3600L;
  }
}

Longitude Longitude::ParseFromMeade(String s)
{
  Longitude result(0.0);
  LOGV2(DEBUG_GENERAL, F("Longitude.Parse(%s)"), s.c_str());

  // Use the DayTime code to parse it.
  DayTime dt = DayTime::ParseFromMeade(s);

  //from indilib driver:  Meade defines longitude as 0 to 360 WESTWARD (https://github.com/indilib/indi/blob/1b2f462b9c9b0f75629b635d77dc626b9d4b74a3/drivers/telescope/lx200driver.cpp#L1019)
  result.totalSeconds = 0 - dt.getTotalSeconds();
  result.checkHours();

  LOGV4(DEBUG_GENERAL, F("Longitude.Parse(%s) -> %s = %ls"), s.c_str(), result.ToString(), result.getTotalSeconds());
  return result;
}

const char *Longitude::formatString(char *targetBuffer, const char *format, long *) const
{
  long secs = totalSeconds;
  if (secs < 0)
  {
    secs += 360L * 3600L;
  }

  return DayTime::formatString(targetBuffer, format, &secs);
}
