#pragma once
#include "inc/Config.hpp"

#include <math.h>

#if USE_GPS == 1

#include <TinyGPS++.h>

#endif

#include "DayTime.hpp"


class Sidereal
{
 public:
 #if USE_GPS == 1
    static DayTime calculateByGPS(TinyGPSPlus* gps);
 #endif

   static DayTime calculateByDriver( double longitude, int year, int month, int day, DayTime *timeUTC );

 private:
    static const double calculateTheta(double deltaJ, double longitude, float timeUTC);
    static const int calculateDeltaJd(int year, int month, int day);
};
