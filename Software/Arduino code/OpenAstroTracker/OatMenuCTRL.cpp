#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/SelectEventArgs.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/Button.hpp"
#include "lib/menu/controls/OptionChooser.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "lib/input/LcdButtons.hpp"

#include "ManualControlModal.hpp"
#include "Mount.hpp"

void setHomePosition(EventArgs *args)
{
    auto mount = Mount::instance();

    SelectEventArgs *arg = (SelectEventArgs *)args;
    if (arg->getSelected().equals("Yes"))
    {
        mount->setHome(true);
        mount->startSlewing(TRACKING);
    }
    args->getSource()->getMainMenu()->closeDialog();
}

void startManualControl(EventArgs *arg)
{
    arg->getSource()->getMainMenu()->activateDialog("RADECControl");
}

void manualControlEvent(ManualControlEventArgs *args)
{
    auto mount = Mount::instance();
    if (args->getEvent() == ManualControlEventArgs::EventType::Close)
    {
        if (args->getSource()->getTag().equals("StartManualControl"))
        {
            args->getSource()->getMainMenu()->activateDialog("StartIsHome");
        }
        else
        {
            args->getSource()->getMainMenu()->activateDialog("ConfirmHome");
        }
    }
    else
    {
        mount->stopSlewing(ALL_DIRECTIONS);
        mount->waitUntilStopped(ALL_DIRECTIONS);
        int keyState = args->getNewState();
        if (keyState == btnNONE)
        {
            // Stop all motors
        }
        else if (keyState == btnLEFT)
        {
            mount->startSlewing(EAST);
        }
        else if (keyState == btnRIGHT)
        {
            mount->startSlewing(WEST);
        }
        else if (keyState == btnUP)
        {
            mount->startSlewing(NORTH);
        }
        else if (keyState == btnDOWN)
        {
            mount->startSlewing(SOUTH);
        }
    }
}

MenuItem ctrlMenu("CTRL", "CTRL");
Button ctrlStart("Manual Control", &startManualControl);
ManualControlModal dlgManualControl(&manualControlEvent, "RADECControl");
OptionChooser yesNoChoice("Yes", "No", 0, &setHomePosition);
MenuItem dlgSetHome("Set home pos?", "ConfirmHome");

void createCTRLMenu(MainMenu &mainMenu)
{
    ctrlMenu.addMenuItem(&ctrlStart);
    dlgSetHome.addMenuItem(&yesNoChoice);
    mainMenu.addMenuItem(&ctrlMenu);
    mainMenu.addModalDialog(&dlgManualControl);
    mainMenu.addModalDialog(&dlgSetHome);
}
