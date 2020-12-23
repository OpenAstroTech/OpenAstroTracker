#include "Sidereal.hpp"


// Constants for sidereal calculation       
// Source: http://www.stargazing.net/kepler/altaz.html
#define C1              100.46
#define C2              0.985647
#define C3              15.0
#define C4              -0.5125
#define J2000           2000

#if USE_GPS == 1
DayTime Sidereal::calculateByGPS(TinyGPSPlus* gps){
    DayTime timeUTC = DayTime(gps->time.hour(), gps->time.minute(), gps->time.second());
    int deltaJd = calculateDeltaJd(gps->date.year(), gps->date.month(), gps->date.day());
    double deltaJ = deltaJd + ((timeUTC.getTotalHours()) / 24.0);
    return DayTime((float)(calculateTheta(deltaJ, gps->location.lng(), timeUTC.getTotalHours()) / 15.0));
    }
#endif // USE_GPS

DayTime Sidereal::calculateByDateAndTime( double longitude, int year, int month, int day, DayTime *timeUTC ) {
    int deltaJd = calculateDeltaJd( year, month, day );
    double deltaJ = deltaJd + ((timeUTC->getTotalHours()) / 24.0);
    return DayTime((float)(calculateTheta(deltaJ, longitude, timeUTC->getTotalHours()) / 15.0));
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


DayTime Sidereal::calculateHa( float lstTotalHours ) {

  float lstDeg = lstTotalHours * 15; //to deg

  //subtract Poloars RA
  lstDeg -= ( ( POLARIS_RA_SECOND / 3600.0f + POLARIS_RA_MINUTE/60.0f + POLARIS_RA_HOUR ) * 15.0f );

  //ensure positive deg
  while( lstDeg < 0.0f ) {
    lstDeg += 360.0f;
  }

  //update HA
  return DayTime( lstDeg / 15.0f );
}