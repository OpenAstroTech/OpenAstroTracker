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
    decMenu->addMenuItem(new NumberInput("TDEC", 4, "^%02d*^%02d\"^%02d' ^@", decConfirmed, decIncr));

    mainMenu.addMenuItem(raMenu);
    mainMenu.addMenuItem(decMenu);
}

void runMenuSystem(MainMenu &mainMenu)
{
    int keyState = LcdButtons::instance()->currentState();

    if (!mainMenu.onPreviewKey(keyState))
    {
        if (mainMenu.processKeys(keyState))
        {
            while (LcdButtons::instance()->currentState() != btnNONE){
                Mount::instance()->delay(20);
            }
                
        }
    }

    mainMenu.updateDisplay();
}