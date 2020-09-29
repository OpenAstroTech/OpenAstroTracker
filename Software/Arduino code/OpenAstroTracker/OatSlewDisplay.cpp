#include "lib/util/debug.hpp"

#include "OatSlewDisplay.hpp"
#include "lib/menu/controls/MainMenu.hpp"
#include "Mount.hpp"

SlewDisplay::SlewDisplay(String displayname, String tag) : ActionRunnerModal(displayname, tag)
{
}

bool SlewDisplay::isComplete()
{
    int slewingAxes;
    float raProgress, decProgress;
    Mount::instance()->getMovementState(slewingAxes, raProgress, decProgress);
    if (slewingAxes == 0){
        LOGV1(255,"SLD: Slew complete!");
    }
    return slewingAxes == 0;
}

void SlewDisplay::onDisplay(bool modal)
{
    char scratchBuffer[20];
    int slewingAxes;
    float raProgress, decProgress;
    auto mount = Mount::instance();
    mount->getMovementState(slewingAxes, raProgress, decProgress);

    String disp;

    if (slewingAxes == (RA_STEPS | DEC_STEPS))
    {
        // Both axes moving to target
        sprintf(scratchBuffer, "R %s %d%%", mount->RAString(LCD_STRING | CURRENT_STRING).c_str(), (int)raProgress);
        getMainMenu()->writeToLCD(0, 0, String(scratchBuffer));
        sprintf(scratchBuffer, "D%s %d%%", mount->DECString(LCD_STRING | CURRENT_STRING).c_str(), (int)decProgress);
        getMainMenu()->writeToLCD(0, 1, String(scratchBuffer));
    }
    else if (slewingAxes == DEC_STEPS)
    {
        // Only DEC moving to target
        sprintf(scratchBuffer, "D%s %d%%", mount->DECString(LCD_STRING | CURRENT_STRING).c_str(), (int)decProgress);
        getMainMenu()->writeToLCD(0, 1, String(scratchBuffer));
    }
    else if (slewingAxes == RA_STEPS)
    {
        // Only RAmoving to target
        sprintf(scratchBuffer, "R %s %d%%", mount->RAString(LCD_STRING | CURRENT_STRING).c_str(), (int)raProgress);
        getMainMenu()->writeToLCD(0, 1, String(scratchBuffer));
    }
    else
    {
        // Nothing moving
        //#if SUPPORT_SERIAL_CONTROL == 1
        // if (inSerialControl)
        // {
        //     sprintf(scratchBuffer, " RA: %s", RAString(LCD_STRING | CURRENT_STRING).c_str());
        //     _lcdDisplay->setCursor(0, 0);
        //     _lcdDisplay->printLine(scratchBuffer);
        //     sprintf(scratchBuffer, "DEC: %s", DECString(LCD_STRING | CURRENT_STRING).c_str());
        //     _lcdDisplay->setCursor(0, 1);
        //     _lcdDisplay->printLine(scratchBuffer);
        // }
        // else
        {
            sprintf(scratchBuffer, "R%s D%s", mount->RAString(COMPACT_STRING | CURRENT_STRING).c_str(), mount->DECString(COMPACT_STRING | CURRENT_STRING).c_str());
            getMainMenu()->writeToLCD(0, 1, String(scratchBuffer));
        }
    }
}