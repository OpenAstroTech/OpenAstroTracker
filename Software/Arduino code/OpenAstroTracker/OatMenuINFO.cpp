#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/ScrollList.hpp"
#include "lib/menu/controls/TextInfo.hpp"
#include "lib/menu/controls/MultiTextInfo.hpp"
#include "configuration.hpp"
#include "raDecIncrementer.hpp"
#include "Mount.hpp"
#if GYRO_LEVEL == 1
#include "Gyro.hpp"
#endif
#include "EPROMStore.hpp"

String getRASteps(int index)
{
    auto mount = Mount::instance();
    switch (index)
    {
    case 0:
        return "RA Stpr: " + String(mount->getCurrentStepperPosition(WEST));
    case 1:
        return "RTrg: " + mount->RAString(LCD_STRING | TARGET_STRING);
    case 2:
        return "RCur: " + mount->RAString(LCD_STRING | CURRENT_STRING);
    }
    return "?";
}

String getDECSteps(int index)
{
    auto mount = Mount::instance();
    switch (index)
    {
    case 0:
          return "DEC Stpr:" + String(mount->getCurrentStepperPosition(NORTH));
    case 1:
          return "DTrg: " + mount->DECString(LCD_STRING | TARGET_STRING);
    case 2:
          return "DCur: " + mount->DECString(LCD_STRING | CURRENT_STRING);
    }
    return "?";
}

String getTRKSteps(int index)
{
    auto mount = Mount::instance();
    switch (index)
    {
    case 0:
        return "TRK Stpr:" + String(mount->getCurrentStepperPosition(TRACKING));
    case 1:
    {
        char scratchBuffer[20];
        sprintf(scratchBuffer, "TRK Spd:");
        dtostrf(mount->getSpeed(TRACKING), 8, 6, &scratchBuffer[8]);
        return String(scratchBuffer);
    }
    break;
    }
    return "?";
}

String getLocation()
{
    char scratchBuffer[20];
    auto mount = Mount::instance();
    float lat = fabs(mount->latitude());
    float lng = fabs(mount->longitude());
    const char dirLat = (mount->latitude() < 0) ? 'S' : 'N';
    const char dirLong = (mount->longitude() < 0) ? 'W' : 'E';
    sprintf(scratchBuffer, "%s%c %s%c", String(lat, 1).c_str(), dirLat, String(lng, 1).c_str(), dirLong);
    return String(scratchBuffer);
}

#if GYRO_LEVEL == 1
String getTemperature()
{
    char scratchBuffer[20];
    int celsius = (int)round(Gyro::getCurrentTemperature());
    int fahrenheit = (int)round(32.0 + 9.0 * Gyro::getCurrentTemperature() / 5.0);
    sprintf(scratchBuffer, "%d@C %d@F", celsius, fahrenheit);
    return String(scratchBuffer);
}
#endif

String getMemAvail()
{
    char scratchBuffer[20];
    sprintf(scratchBuffer, "%d", freeMemory());
    return String(scratchBuffer);
}

String getUpTime()
{
    char scratchBuffer[20];
    long now = millis();
    long msPerDay = 60L * 60 * 24 * 1000;
    int days = (int)(now / msPerDay);
    now -= days * msPerDay;
    DayTime elapsed(now);
    sprintf(scratchBuffer, "%dd %02d:%02d:%02d", days, elapsed.getHours(), elapsed.getMinutes(), elapsed.getSeconds());
    return String(scratchBuffer);
}

String getFirmware()
{
    return String(Version);
}

MenuItem infoMenu("INFO", "INFO");
ScrollList infoList;
MultiTextInfo raSteps(3, &getRASteps);
MultiTextInfo decSteps(3, &getDECSteps);
MultiTextInfo trkSteps(2, &getTRKSteps);
TextInfo location("Loc ", &getLocation);
#if GYRO_LEVEL == 1
TextInfo temperature("Temp: ", &getTemperature);
#endif
TextInfo memory("MemAvail: ", &getMemAvail);
TextInfo uptime( "Up: ", &getUpTime);
TextInfo firmware("Firmw.: ", &getFirmware);

void createINFOMenu(MainMenu &mainMenu)
{
    infoList.addMenuItem(&raSteps);
    infoList.addMenuItem(&decSteps);
    infoList.addMenuItem(&trkSteps);
    infoList.addMenuItem(&location);
#if GYRO_LEVEL == 1
    infoList.addMenuItem(&temperature);
#endif
    infoList.addMenuItem(&memory);
    infoList.addMenuItem(&uptime);
    infoList.addMenuItem(&firmware);

    infoMenu.addMenuItem(&infoList);
    mainMenu.addMenuItem(&infoMenu);
}
