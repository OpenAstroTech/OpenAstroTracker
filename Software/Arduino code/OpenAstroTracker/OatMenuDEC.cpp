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

    args->getSource()->getMainMenu()->activateDialog("SlewDisplay");
    mount->startSlewingToTarget();
}

RaDecIncrementer decIncr ("DEC");
MenuItem decMenu ("DEC","DEC");
NumberInput decNumber("TDEC", 4, "^%+02d*^%02d\"^%02d'^@", decConfirmed, &decIncr);

void createDECMenu(MainMenu &mainMenu)
{
    decMenu.addMenuItem(&decNumber);
    mainMenu.addMenuItem(&decMenu);
}
