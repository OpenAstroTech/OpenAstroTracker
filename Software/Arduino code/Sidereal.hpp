#pragma once

#include <math.h>
#include <TinyGPS++.h>

#include "DayTime.hpp"


class Sidereal
{
 public:
    static DayTime calculateByGPS(TinyGPSPlus* gps);

 private:
    static const double calculateTheta(double deltaJ, double longitude, float timeUTC);
    static const int calculateDeltaJd(int year, int month, int day);
};
