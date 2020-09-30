#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/Button.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "lib/input/LcdButtons.hpp"
#include "configuration.hpp"
#include "raDecIncrementer.hpp"
#include "Mount.hpp"
#include "EPROMStore.hpp"

#include "OatMenuRA.hpp"
#include "OatMenuDEC.hpp"
#include "OatMenuGO.hpp"
#include "OatMenuHA.hpp"
#include "OatMenuCTRL.hpp"
#include "OatSlewDisplay.hpp"

//RAM:   [=======   ]  67.6% (used 5537 bytes from 8192 bytes)
//Flash: [==        ]  21.0% (used 54850 bytes from 261120 bytes)

//RAM:   [=======   ]  67.9% (used 5561 bytes from 8192 bytes)
//Flash: [==        ]  21.0% (used 54876 bytes from 261120 bytes)
SlewDisplay slewModal("SLD","SlewDisplay");

void createMenuSystem(MainMenu &mainMenu)
{
    createRAMenu(mainMenu);
    createDECMenu(mainMenu);
    createGOMenu(mainMenu);
    createHAMenu(mainMenu);
    createCTRLMenu(mainMenu);
    mainMenu.addModalDialog(&slewModal);
    LOGV1(DEBUG_INFO, "CMS: Menu system created");
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

    //LOGV1(DEBUG_ANY, "RMS: done");
}