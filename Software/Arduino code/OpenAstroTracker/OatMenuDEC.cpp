#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "raDecIncrementer.hpp"
#include "Mount.hpp"


void decConfirmed(EventArgs *args)
{
    auto mount = Mount::instance();
    if (mount->isSlewingRAorDEC())
    {
        mount->stopSlewing(ALL_DIRECTIONS);
        mount->waitUntilStopped(ALL_DIRECTIONS);
    }

    mount->startSlewingToTarget();
}

void createDECMenu(MainMenu &mainMenu)
{
    auto decIncr = new RaDecIncrementer("DEC");
    auto decMenu = new MenuItem("DEC");
    decMenu->addMenuItem(new NumberInput("TDEC", 4, "^%+02d*^%02d\"^%02d'^@", decConfirmed, decIncr));
    mainMenu.addMenuItem(decMenu);
}
