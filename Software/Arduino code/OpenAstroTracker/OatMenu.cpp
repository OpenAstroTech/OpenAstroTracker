#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/Button.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "lib/input/LcdButtons.hpp"
#include "raDecIncrementer.hpp"
#include "Mount.hpp"

void raConfirmed(EventArgs *args)
{
    auto num = (NumberInput *)args->getSource();
    Mount::instance()->targetRA().set(num->getValue(0), num->getValue(1), num->getValue(2));
}

void decConfirmed(EventArgs *args)
{
    auto num = (NumberInput *)args->getSource();
    Mount::instance()->targetDEC().set(num->getValue(0), num->getValue(1), num->getValue(2));
}

void createMenuSystem(MainMenu &mainMenu)
{
    auto raIncr = new RaDecIncrementer("RA");
    auto decIncr = new RaDecIncrementer("DEC");

    //////  RA  ///////
    auto raMenu = new MenuItem("RA");
    raMenu->addMenuItem(new NumberInput(String("TRA"), 4, String("^%02dh^%02dm^%02ds ^@"), raConfirmed, raIncr));

    //////  DEC  ///////
    auto decMenu = new MenuItem("DEC");
    decMenu->addMenuItem(new NumberInput("TDEC", 4, "^%+02d*^%02d\"^%02d'^@", decConfirmed, decIncr));

    mainMenu.addMenuItem(raMenu);
    mainMenu.addMenuItem(decMenu);
}

long lastUpdate = 0;
void runMenuSystem(MainMenu &mainMenu)
{
    delay(20);
    int keyState = LcdButtons::instance()->currentState();

    if (!mainMenu.onPreviewKey(keyState))
    {
        LOGV1(DEBUG_ANY, "RMS: No preview key ");
        if (mainMenu.processKeys(keyState))
        {
            delay(20);
            LOGV1(DEBUG_ANY, "RMS: Preview used, wait for release");
            while (LcdButtons::instance()->currentState() != btnNONE)
            {
                Mount::instance()->delay(20);
            }
            LOGV1(DEBUG_ANY, "RMS: released");
        }
    }

    if (millis() - lastUpdate > 200)
    {
        LOGV1(DEBUG_ANY, "RMS: updating menu");
        mainMenu.updateDisplay();
        lastUpdate = millis();
    }

    LOGV1(DEBUG_ANY, "RMS: done");
}