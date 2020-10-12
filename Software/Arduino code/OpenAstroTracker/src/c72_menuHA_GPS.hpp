#pragma once

#include "EPROMStore.hpp"
#include "../Configuration_adv.hpp"
#include "../Configuration_pins.hpp"

#if USE_GPS == 1

bool gpsAqcuisitionComplete(int & indicator)
{
    while (GPS_SERIAL_PORT.available())
    {
        int gpsChar = GPS_SERIAL_PORT.read();
        if (gpsChar == 36)
        {
            // $ (ASCII 36) marks start of message, so we switch indicator every message
            indicator = adjustWrap(indicator, 1, 0, 3);
        }
        if (gps.encode(gpsChar))
        {
            // Make sure we got a fix in the last 60seconds
            if ((gps.location.lng() != 0) && (gps.location.age() < 60000UL))
            {
                LOGV2(DEBUG_INFO, F("Found GPS location. Age is %d secs"), gps.location.age() / 1000);
                LOGV4(DEBUG_INFO, F("GPS UTC is %dh%dm%ds"), gps.time.hour(), gps.time.minute(), gps.time.second());
                lcdMenu.printMenu("GPS sync'd....");

                DayTime lst = Sidereal::calculateByGPS(&gps);
                int hHAfromLST = lst.getHours() - POLARIS_RA_HOUR;
                int mHAfromLST = lst.getMinutes() - POLARIS_RA_MINUTE;
                mount.setHA(DayTime(hHAfromLST, mHAfromLST, 0));

                EPROMStore::update(1, mount.HA().getHours());
                EPROMStore::update(2, mount.HA().getMinutes());
                mount.setLatitude(gps.location.lat());
                mount.setLongitude(gps.location.lng());

                mount.delay(500);

                return true;
            }
        }
    }
    return false;
}

#if HEADLESS_CLIENT == 0

// States that HA menu displays goes through
#define SHOWING_HA_SYNC 1
#define SHOWING_HA_SET 2
#define ENTER_HA_MANUALLY 3
#define STARTING_GPS 4
#define SIGNAL_AQCUIRED 5

int indicator = 0;
int haState = STARTING_GPS;

bool processHAKeys()
{
    byte key;
    bool waitForRelease = false;

    if (haState == STARTING_GPS)
    {
        if (gpsAqcuisitionComplete(indicator))
        {
            LOGV1(DEBUG_INFO, F("HA: GPS acquired"));
            GPS_SERIAL_PORT.end();
            haState = SHOWING_HA_SYNC;
            if (startupState == StartupWaitForHACompletion)
            {
                LOGV1(DEBUG_INFO, F("HA: We were in startup, so confirm HA"));
                startupState = StartupHAConfirmed;
                inStartup = true;
            }
        }
    }

    if (lcdButtons.keyChanged(&key))
    {
        waitForRelease = true;
        LOGV3(DEBUG_INFO, F("HA: Key %d was pressed in state %d"), key, haState);
        if (haState == SHOWING_HA_SYNC)
        {
            if (key == btnSELECT)
            {
                haState = STARTING_GPS;
                GPS_SERIAL_PORT.begin(GPS_BAUD_RATE);
            }
            else if ((key == btnUP) || (key == btnDOWN))
            {
                haState = SHOWING_HA_SET;
            }
        }
        else if (haState == SHOWING_HA_SET)
        {
            if (key == btnSELECT)
            {
                haState = ENTER_HA_MANUALLY;
            }
            else if ((key == btnUP) || (key == btnDOWN))
            {
                haState = SHOWING_HA_SYNC;
            }
        }
        else if (haState == ENTER_HA_MANUALLY)
        {
            if (key == btnSELECT)
            {
                DayTime ha(mount.HA());
                EPROMStore::update(1, mount.HA().getHours());
                EPROMStore::update(2, mount.HA().getMinutes());
                lcdMenu.printMenu("Stored.");
                mount.delay(500);
                haState = SHOWING_HA_SET;
                if (startupState == StartupWaitForHACompletion)
                {
                    startupState = StartupHAConfirmed;
                    inStartup = true;
                }
            }
            else if (key == btnUP)
            {
                DayTime ha(mount.HA());
                if (HAselect == 0)
                    ha.addHours(1);
                if (HAselect == 1)
                    ha.addMinutes(1);
                mount.setHA(ha);
            }
            else if (key == btnDOWN)
            {
                DayTime ha(mount.HA());
                if (HAselect == 0)
                    ha.addHours(-1);
                if (HAselect == 1)
                    ha.addMinutes(-1);
                mount.setHA(ha);
            }
            else if (key == btnLEFT)
            {
                HAselect = adjustWrap(HAselect, 1, 0, 1);
            }
        }

        if (key == btnRIGHT)
        {
            LOGV1(DEBUG_INFO, F("HA: Right Key was pressed"));
            if (haState == STARTING_GPS)
            {
                LOGV1(DEBUG_INFO, F("HA: In GPS Start mode, switching to manual"));
                GPS_SERIAL_PORT.end();
                haState = SHOWING_HA_SYNC;
            }
            else if (startupState == StartupWaitForHACompletion)
            {
                LOGV1(DEBUG_INFO, F("HA: In Startup, not in GPS Start mode, leaving"));
                startupState = StartupHAConfirmed;
                inStartup = true;
            }
            else
            {
                LOGV1(DEBUG_INFO, F("HA: leaving HA"));
                lcdMenu.setNextActive();
            }
        }
    }

    return waitForRelease;
}

void printHASubmenu()
{
    const char *ind = "**  ";
    char satBuffer[20];
    if (haState == SHOWING_HA_SYNC)
    {
        sprintf(satBuffer, "%02dh %02dm >Sync", mount.HA().getHours(), mount.HA().getMinutes());
    }
    else if (haState == SHOWING_HA_SET)
    {
        sprintf(satBuffer, "%02dh %02dm >Set", mount.HA().getHours(), mount.HA().getMinutes());
    }
    else if (haState == STARTING_GPS)
    {
        sprintf(satBuffer, "  Found %lu sats", gps.satellites.value());
        satBuffer[0] = ind[indicator];
    }
    else if (haState == ENTER_HA_MANUALLY)
    {
        sprintf(satBuffer, " %02dh %02dm", mount.HA().getHours(), mount.HA().getMinutes());
        satBuffer[HAselect * 4] = '>';
    }
    lcdMenu.printMenu(satBuffer);
}

#endif
#endif
