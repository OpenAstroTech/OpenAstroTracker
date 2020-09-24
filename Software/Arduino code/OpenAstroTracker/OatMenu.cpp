#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/Button.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "lib/input/LcdButtons.hpp"
#include "raDecIncrementer.hpp"
#include "Mount.hpp"
#include "EPROMStore.hpp"

int hourLimits[] = {23, 59, 59};

void raOrDecConfirmed(EventArgs *args)
{
    //auto num = (NumberInput *)args->getSource();
    auto mount = Mount::instance();
    //mount->targetRA().set(num->getValue(0), num->getValue(1), num->getValue(2));
    if (mount->isSlewingRAorDEC())
    {
        mount->stopSlewing(ALL_DIRECTIONS);
        mount->waitUntilStopped(ALL_DIRECTIONS);
    }

    mount->startSlewingToTarget();
}

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
    EPROMStore::Storage()->update(1, mount->HA().getHours());
    EPROMStore::Storage()->update(2, mount->HA().getMinutes());
    LcdDisplay::instance()->setCursor(0, 1);
    LcdDisplay::instance()->printLine("Stored.");
    mount->delay(500);
}

void createMenuSystem(MainMenu &mainMenu)
{
    auto mount = Mount::instance();
    auto raIncr = new RaDecIncrementer("RA");
    auto decIncr = new RaDecIncrementer("DEC");
    auto hourIncr = new Incrementer(INCREMENT_WRAP, hourLimits, nullptr, haChanged);

    //////  RA  ///////
    auto raMenu = new MenuItem("RA");
    raMenu->addMenuItem(new NumberInput("TRA", 4, "^%02dh^%02dm^%02ds ^@", raOrDecConfirmed, raIncr));

    //////  DEC  ///////
    auto decMenu = new MenuItem("DEC");
    decMenu->addMenuItem(new NumberInput("TDEC", 4, "^%+02d*^%02d\"^%02d'^@", raOrDecConfirmed, decIncr));

    //////  HA  ///////
    auto haMenu = new MenuItem("HA");
    int *haVal = new int[2];
    haVal[0] = mount->HA().getHours();
    haVal[1] = mount->HA().getMinutes();
    haMenu->addMenuItem(new NumberInput("HA", haVal, 2, "^%02dh^%02dm", haConfirmed, hourIncr));

    mainMenu.addMenuItem(raMenu);
    mainMenu.addMenuItem(decMenu);
    mainMenu.addMenuItem(haMenu);
}

long lastUpdate = 0;
void runMenuSystem(MainMenu &mainMenu)
{
    int keyState = LcdButtons::instance()->currentState();

    if (!mainMenu.onPreviewKey(keyState))
    {
        //LOGV1(DEBUG_ANY, "RMS: No preview key ");
        if (mainMenu.processKeys(keyState))
        {
            delay(20);
            //LOGV1(DEBUG_ANY, "RMS: Preview used, wait for release");
            while (LcdButtons::instance()->currentState() != btnNONE)
            {
                Mount::instance()->delay(20);
            }
            //LOGV1(DEBUG_ANY, "RMS: released");
        }
    }

    if (millis() - lastUpdate > 200)
    {
        //LOGV1(DEBUG_ANY, "RMS: updating menu");
        mainMenu.updateDisplay();
        lastUpdate = millis();
    }

    //LOGV1(DEBUG_ANY, "RMS: done");
}