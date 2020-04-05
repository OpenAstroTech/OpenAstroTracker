#ifndef _DAYTIME_HPP_
#define _DAYTIME_HPP_

#include <Arduino.h>
#include "Globals.h"

// A class to handle hours, minutes, seconds in a unified manner, allowing
// addition of hours, minutes, seconds, other times and conversion to string.

class DayTime {
  protected:
    int hours;
    int mins;
    int secs;
    int hourWrap = 24;

  public:
    DayTime();

    DayTime(const DayTime& other);
    DayTime(int h, int m, int s);

    // From milliseconds. Does not handle days!
    DayTime(long ms);

    // From hours
    DayTime(float timeInHours);

    int getHours();
    int getMinutes();
    int getSeconds();
    float getTotalHours();
    float getTotalMinutes();
    float getTotalSeconds();

    int getTime(int& h, int& m, int& s);
    void set(int h, int m, int s);
    void set(const DayTime& other);

    // Add hours, wrapping days (which are not tracked). Negative or positive.
    virtual void addHours(int deltaHours);

    // Add minutes, wrapping hours if needed
    void addMinutes(int deltaMins);

    // Add seconds, wrapping minutes and hours if needed
    void addSeconds(long deltaSecs);

    // Add time components, wrapping seconds, minutes and hours if needed
    void addTime(int deltaHours, int deltaMinutes, int deltaSeconds);

    // Add another time, wrapping seconds, minutes and hours if needed
    void addTime(const DayTime& other);
    // Subtract another time, wrapping seconds, minutes and hours if needed

    void subtractTime(const DayTime& other);

    // Convert to a standard string (like 14:45:06)
    String ToString();
  //protected:
    virtual void checkHours();
};

class DegreeTime : public DayTime {
  public:
    DegreeTime();
    DegreeTime(const DegreeTime& other);
    DegreeTime(int h, int m, int s);
    DegreeTime(float inDegrees);

    // Add degrees, clamp at 90
    void addDegrees(int deltaDegrees);

    // Get degrees component
    int getDegrees();

    // Get degrees for printing component
    int getPrintDegrees();

    // Get total degrees
    float getTotalDegrees();
  //protected:
    virtual void checkHours() override;

  private:
    void clampDegrees();
};

#endif
