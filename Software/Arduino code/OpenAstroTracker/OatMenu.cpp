#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/Button.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "lib/input/LcdButtons.hpp"
#include "configuration.hpp"
#include "StringTable.hpp"
#include "raDecIncrementer.hpp"
#include "Mount.hpp"
#include "EPROMStore.hpp"

#include "OatMenuRA.hpp"
#include "OatMenuDEC.hpp"
#include "OatMenuGO.hpp"
#include "OatMenuHA.hpp"
#include "OatMenuCTRL.hpp"
#include "OatMenuCAL.hpp"
#include "OatMenuINFO.hpp"
#include "OatSlewDisplay.hpp"

SlewDisplay slewModal("SLD",oatString(DLG_SLEW_DISPLAY));

void createMenuSystem(MainMenu &mainMenu)
{
    LOGV1(DEBUG_INFO, F("CMS: Create RA"));
    createRAMenu(mainMenu);
    LOGV1(DEBUG_INFO, F("CMS: Create DEC"));
    createDECMenu(mainMenu);
    
    LOGV1(DEBUG_INFO, F("CMS: Create GO"));
    createGOMenu(mainMenu);
    
    LOGV1(DEBUG_INFO, F("CMS: Create HA"));
    createHAMenu(mainMenu);

    #if SUPPORT_MANUAL_CONTROL == 1
    LOGV1(DEBUG_INFO, F("CMS: Create CTRL"));
    createCTRLMenu(mainMenu);
    #endif

    #if SUPPORT_CALIBRATION == 1
    LOGV1(DEBUG_INFO, F("CMS: Create CAL"));
    createCALMenu(mainMenu);
    #endif

    #if SUPPORT_INFO_DISPLAY == 1
    LOGV1(DEBUG_INFO, F("CMS: Create INFO"));
    createINFOMenu(mainMenu);
    #endif

    LOGV1(DEBUG_INFO, F("CMS: Add Slew modal"));
    mainMenu.addModalDialog(&slewModal);
    LOGV1(DEBUG_INFO, F("CMS: Menu system created"));
}

long lastUpdate = 0;
void runMenuSystem(MainMenu &mainMenu)
{
    int keyState = LcdButtons::instance()->currentKey();

    if (!mainMenu.onPreviewKey(keyState))
    {
        if (mainMenu.processKeys(keyState))
        {
            //LOGV1(DEBUG_ANY, "RMS: ProcessKeys used, wait for release");
            while (LcdButtons::instance()->currentKey() != btnNONE)
            {
                Mount::instance()->delay(10);
            }
            //LOGV1(DEBUG_ANY, "RMS: released");
        }
    }
    else
    {
        // LOGV1(DEBUG_ANY, "RMS: Preview key processed!");
    }

    if (millis() - lastUpdate > 200)
    {
        //LOGV1(DEBUG_ANY, "RMS: updating menu");
        mainMenu.updateDisplay();
        lastUpdate = millis();
    }
   
}