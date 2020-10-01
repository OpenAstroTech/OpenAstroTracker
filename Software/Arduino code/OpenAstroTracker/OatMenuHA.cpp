#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "raDecIncrementer.hpp"
#include "Mount.hpp"
#include "EPROMStore.hpp"


int hourLimits[] = {23, 59, 59};
int haVal[] = {0, 0};

void haChanged(EventArgs *args)
{
    auto changedArgs = (ChangedEventArgs *)args;
    auto mount = Mount::instance();
    DayTime ha(mount->HA());
    ha.set(changedArgs->getNumbers()[0], changedArgs->getNumbers()[1], 0);
    mount->setHA(ha);
}

void haConfirmed(EventArgs *args)
{
    auto mount = Mount::instance();
    EPROMStore::update(1, mount->HA().getHours());
    EPROMStore::update(2, mount->HA().getMinutes());
    LcdDisplay::instance()->setCursor(0, 1);
    LcdDisplay::instance()->printLine("Stored.");
    mount->delay(500);
}

MenuItem haMenu("HA", "HA");
Incrementer hourIncr(INCREMENT_WRAP, hourLimits, nullptr, haChanged);
NumberInput haInput("HA", haVal, 2, "^%02dh^%02dm", haConfirmed, &hourIncr);

void createHAMenu(MainMenu &mainMenu)
{
    auto mount = Mount::instance();
    haVal[0] = mount->HA().getHours();
    haVal[1] = mount->HA().getMinutes();
    haMenu.addMenuItem(&haInput);
    mainMenu.addMenuItem(&haMenu);
}
