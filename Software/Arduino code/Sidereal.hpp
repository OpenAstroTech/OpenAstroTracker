#pragma once

#include <math.h>
#include <TinyGPS++.h>

#include "DayTime.hpp"

// Constants for sidereal calculation       
// Source: http://www.stargazing.net/kepler/altaz.html
#define C1              100.46
#define C2              0.985647
#define C3              15.0
#define C4              -0.5125
#define J2000           2000


class Sidereal
{
 public:
    static DayTime calculateByGPS(TinyGPSPlus* gps);

 private:
    static const double calculateTheta(double deltaJ, double longitude, float timeUTC);
    static const int calculateDeltaJd(int year, int month, int day);
};
