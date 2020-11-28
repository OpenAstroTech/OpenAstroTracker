#include "Utility.hpp"
#include "Latitude.hpp"

//////////////////////////////////////////////////////////////////////////////////////
//
// 90 is north pole, -90 is south pole
Latitude::Latitude(const Latitude &other) : DayTime(other)
{
}

Latitude::Latitude(int h, int m, int s) : DayTime(h, m, s)
{
}

Latitude::Latitude(float inDegrees) : DayTime(inDegrees)
{
}

void Latitude::checkHours()
{
  if (totalSeconds > 90L * 3600L)
  {
    LOGV1(DEBUG_GENERAL, F("LatitudeCheckHours: Degrees is more than 90, clamping"));
    totalSeconds = 90L * 3600L;
  }
  if (totalSeconds < (-90L * 3600L))
  {
    LOGV1(DEBUG_GENERAL, F("LatitudeCheckHours: Degrees is less than -90, clamping"));
    totalSeconds = -90L * 3600L;
  }
}

Latitude Latitude::ParseFromMeade(String s)
{
  Latitude result(0.0);

  LOGV2(DEBUG_GENERAL, F("Latitude.Parse(%s)"), s.c_str());
  // Use the DayTime code to parse it.
  DayTime dt = DayTime::ParseFromMeade(s);
  result.totalSeconds = dt.getTotalSeconds();
  result.checkHours();
  LOGV3(DEBUG_GENERAL, F("Latitude.Parse(%s) -> %s"), s.c_str(), result.ToString());
  return result;
}
