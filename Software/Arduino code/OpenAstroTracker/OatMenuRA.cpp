#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/NumberInput.hpp"
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

    mount->startSlewingToTarget();
}

void createRAMenu(MainMenu &mainMenu)
{
    auto raIncr = new RaDecIncrementer("RA");
    auto raMenu = new MenuItem("RA");
    raMenu->addMenuItem(new NumberInput("TRA", 4, "^%02dh^%02dm^%02ds ^@", raConfirmed, raIncr));
    mainMenu.addMenuItem(raMenu);
}
