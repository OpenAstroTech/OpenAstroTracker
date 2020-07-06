#include "Sidereal.hpp"

static DayTime Sidereal::calculateByGPS(TinyGPSPlus* gps){
    DayTime timeUTC = DayTime(gps->time.hour(), gps->time.minute(), gps->time.second());
    int deltaJd = calculateDeltaJd(gps->date.year(), gps->date.month(), gps->date.day());
    double deltaJ = deltaJd + ((timeUTC.getTotalHours()) / 24.0);
    return DayTime((float)(calculateTheta(deltaJ, gps->location.lng(), timeUTC.getTotalHours()) / 15.0));
}

static const double Sidereal::calculateTheta(double deltaJ, double longitude, float timeUTC){
    float theta = C1;
    theta += C2 * deltaJ;
    theta += C3 * timeUTC;
    theta += longitude;
    return fmod(theta, 360.0);
}

static const int Sidereal::calculateDeltaJd(int year, int month, int day){
    const int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Calculating days without leapdays
    long int deltaJd = (year - REFERENCE_JEAR) * 365 + day; 
    for (int i=0; i < month - 1; i++){
        deltaJd += daysInMonth[i];
    }

    // Add leapdays
    if (month <= 2){
        year--;
    }
    deltaJd += (year / 4 - year / 100 + year / 400);
    deltaJd -= (REFERENCE_JEAR / 4 - REFERENCE_JEAR / 100 + REFERENCE_JEAR / 400);
    return deltaJd; 
}
