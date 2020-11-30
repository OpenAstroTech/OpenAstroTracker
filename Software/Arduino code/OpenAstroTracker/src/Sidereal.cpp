#include "Sidereal.hpp"

#if USE_GPS == 1

// Constants for sidereal calculation       
// Source: http://www.stargazing.net/kepler/altaz.html
#define C1              100.46
#define C2              0.985647
#define C3              15.0
#define C4              -0.5125
#define J2000           2000

DayTime Sidereal::calculateByGPS(TinyGPSPlus* gps){
    DayTime timeUTC = DayTime(gps->time.hour(), gps->time.minute(), gps->time.second());
    //DayTime timeUTC = DayTime(14, 24, 11);
    int deltaJd = calculateDeltaJd(gps->date.year(), gps->date.month(), gps->date.day());
    //int deltaJd = calculateDeltaJd(2020, 7, 15);
    double deltaJ = deltaJd + ((timeUTC.getTotalHours()) / 24.0);
    return DayTime((float)(calculateTheta(deltaJ, gps->location.lng(), timeUTC.getTotalHours()) / 15.0));
    //return DayTime((float)(calculateTheta(deltaJ, 8.487581, timeUTC.getTotalHours()) / 15.0));
    }

const double Sidereal::calculateTheta(double deltaJ, double longitude, float timeUTC){
    double theta = C1;
    theta += C2 * deltaJ;
    theta += C3 * timeUTC;
    theta += C4;
    theta += longitude;
    return fmod(theta, 360.0);
}

const int Sidereal::calculateDeltaJd(int year, int month, int day){
    const int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Calculating days without leapdays
    long int deltaJd = (year - J2000) * 365 + day; 
    for (int i=0; i < month - 1; i++){
        deltaJd += daysInMonth[i];
    }

    // Add leapdays
    if (month <= 2){
        // Not counting current year if we have not passed february yet
        year--;
    }
    deltaJd += year / 4 - year / 100 + year / 400;
    deltaJd -= J2000 / 4 - J2000 / 100 + J2000 / 400;
    return deltaJd; 
}

#endif // USE_GPS
