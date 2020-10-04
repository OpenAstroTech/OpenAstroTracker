#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "StringTable.hpp"
#include "raDecIncrementer.hpp"
#include "Mount.hpp"

void raConfirmed(EventArgs *args)
{
    auto mount = Mount::instance();
    if (mount->isSlewingRAorDEC())
    {
        mount->stopSlewing(ALL_DIRECTIONS);
        mount->waitUntilStopped(ALL_DIRECTIONS);
    }
    args->getSource()->getMainMenu()->activateDialog(oatString(DLG_SLEW_DISPLAY));
    mount->startSlewingToTarget();
}

RaDecIncrementer raIncr("RA");
MenuItem raMenu("RA", "RA");
NumberInput raNumber("TRA", 4, "^%02dh^%02dm^%02ds ^@", raConfirmed, &raIncr);

void createRAMenu(MainMenu &mainMenu)
{
    raMenu.addMenuItem(&raNumber);
    mainMenu.addMenuItem(&raMenu);
}
