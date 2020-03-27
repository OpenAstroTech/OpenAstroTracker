// A class to handle hours, minutes, seconds in a unified manner, allowing
// addition of hours, minutes, seconds, other times and conversion to string.
class DayTime {
  private:
    int hours;
    int mins;
    int secs;

  public:
    DayTime() {
      hours = 0;
      mins = 0;
      secs = 0;
    }

    DayTime(const DayTime& other) {
      hours = other.hours;
      mins = other.mins;
      secs = other.secs;
    }

    DayTime(int m, int h, int s) {
      hours = h;
      mins = m;
      secs = s;
    }

    int getHours() {
      return hours;
    }

    int getMinutes() {
      return mins;
    }

    int getSeconds() {
      return secs;
    }

    float getTotalHours() {
      return 1.0f * getHours() + ((float)getMinutes() / 60.0f) + ((float)getSeconds() / 3600.0f);
    }

    float getTotalMinutes() {
      return 60.0f * getHours() + (float)getMinutes() + ((float)getSeconds() / 60.0f);
    }

    float getTotalSeconds() {
      return 3600.0f * getHours() + (float)getMinutes() * 60.0f + (float)getSeconds();
    }

    int getTime(int& h, int& m, int& s) {
      h = hours;
      m = mins;
      s = secs;
    }

    void set(int h, int m, int s) {
      hours = h;
      mins = m;
      secs = s;
    }

    void set(const DayTime& other) {
      hours = other.hours;
      mins = other.mins;
      secs = other.secs;
    }

    // Add hours, wrapping days (which are not tracked)
    void addHours(int deltaHours) {
      hours += deltaHours;
      while (hours > 23) {
        hours  -= 24;
      }
      while (hours < 0) {
        hours += 24;
      }
    }

    // Add minutes, wrapping hours if needed
    void addMinutes(int deltaMins) {
      mins += deltaMins;
      while (mins > 59) {
        mins  -= 60;
        addHours(1);
      }
      while (mins  < 0) {
        mins  += 60;
        addHours(-1);
      }
    }

    // Add seconds, wrapping minutes and hours if needed
    void addSeconds(int deltaSecs) {
      secs += deltaSecs;
      while (secs > 59) {
        secs -= 60;
        addMinutes(1);
      }
      while (secs < 0) {
        secs += 60;
        addMinutes(-1);
      }
    }

    // Add time components, wrapping seconds, minutes and hours if needed
    void addTime(int deltaHours, int deltaMinutes, int deltaSeconds)
    {
      addSeconds(deltaSeconds);
      addMinutes(deltaMinutes);
      addHours(deltaHours);
    }

    // Add another time, wrapping seconds, minutes and hours if needed
    void addTime(const DayTime& other)
    {
      addSeconds(other.getSeconds());
      addMinutes(other.getMinutes());
      addHours(other.getHours());
    }

    // Convert to a standard string (like 14:45:06)
    String ToString()
    {
      char achBuf[10];
      char*p = achBuf;
      if (hours < 10) {
        *p++ = '0';
      } else {
        *p++ = '0' + (hours / 10);
      }
      *p++ = '0' + (hours % 10);
      *p++ = ':';
      if (mins < 10) {
        *p++ = '0';
      } else {
        *p++ = '0' + (mins / 10);
      }
      *p++ = '0' + (mins % 10);
      *p++ = ':';
      if (secs < 10) {
        *p++ = '0';
      } else {
        *p++ = '0' + (secs / 10);
      }
      *p++ = '0' + (secs % 10);
      *p++ = '\0';
      return String(achBuf);
    }
};
