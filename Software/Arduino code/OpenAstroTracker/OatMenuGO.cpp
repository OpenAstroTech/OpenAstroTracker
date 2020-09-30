#include <Arduino.h>
#include <string.h>

#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "lib/menu/controls/ScrollList.hpp"
#include "lib/menu/controls/Button.hpp"
#include "lib/menu/controls/SelectEventArgs.hpp"
#include "raDecIncrementer.hpp"
#include "configuration.hpp"
#include "Mount.hpp"

ScrollList goList;
MenuItem goMenu("GO", "GO");

struct PointOfInterest
{
    const char *pDisplay;
    byte hourRA;
    byte minRA;
    byte secRA;
    int degreeDEC;
    byte minDEC;
    byte secDEC;
};

// Points of interest are sorted by DEC
PointOfInterest pointOfInterest[] = {
   //    Name (15chars)    RA (hms)     DEC (dms)
   //  012345678901234
    { "Polaris"           ,  PolarisRAHour, PolarisRAMinute, PolarisRASecond,  89, 21,  6 },
    { "Veil Nebula"     ,20, 51, 28,   30, 59, 30 },
	{ "M81 Bodes Galxy" , 9, 57, 13,   68, 58,  1 },
	{ "Cederblad 214"   , 0,  5, 46,   67, 16, 45 },
	{ "Heart Nebula"    , 2, 34, 57,   61, 31, 17 },
	{ "Navi"            , 0, 57, 57,   60, 49, 33 },
	{ "Soul Nebula"     , 2, 52, 47,   60, 30, 56 },
	{ "Elephant Trunk"  ,21, 39, 44,   57, 35, 31 },
	{ "Big Dipper"      ,12, 16, 26,   56, 55,  7 },
	{ "M101 Pinwheel"   ,14,  3, 56,   54, 15,  0 },
	{ "M51 Whirlpool"   ,13, 30, 45,   47,  5, 21 },
	{ "Deneb (Cygnus)"  ,20, 42,  7,   45, 21, 12 },
	{ "M63 Sunflower"   ,13, 16, 45,   41, 55, 14 },
	{ "M31 Andromeda"   , 0, 43, 52,   41, 22, 53 },
	{ "Vega"            ,18, 37, 37,   38, 48,  7 },
	{ "Arcturus"        ,14, 16, 37,   19,  5, 21 },
	{ "Altair"          ,19, 51, 45,    8, 55, 15 },
	{ "M42 Orion Nbula" , 5, 36, 18,   -5, 22, 44 },
	{ "Lagoon Nebula"   ,18,  5,  2,  -24, 22, 52 },

    // Add new items above here, not below.
    { "Home"           ,  0,  0,  0,   90,  0,  0 },
    { "Unpark"         ,  0,  0,  0,   90,  0,  0 },
    { "Park"           ,  0,  0,  0,   90,  0,  0 },
    // And definitely don't add here.
};

void goPointChosen(EventArgs *args)
{
    auto selectArg = (SelectEventArgs *)args;

    auto mount = Mount::instance();
    mount->stopSlewing(ALL_DIRECTIONS);

    if (selectArg->getSelected().equals("Home"))
    {
        mount->goHome();
    }
    if (selectArg->getSelected().equals("Park"))
    {
        mount->park();
    }
    if (selectArg->getSelected().equals("Unpark"))
    {
        mount->startSlewing(TRACKING);
    }
    else
    {
        for (unsigned int i = 0; i < sizeof(pointOfInterest) / sizeof(pointOfInterest[0]); i++)
        {
            auto poi = &pointOfInterest[i];
            if (selectArg->getSelected().equals(poi->pDisplay))
            {
                mount->targetRA().set(poi->hourRA, poi->minRA, poi->secRA);
                mount->targetDEC().set(poi->degreeDEC - (NORTHERN_HEMISPHERE ? 90 : -90), poi->minDEC, poi->secDEC); // internal DEC degree is 0 at celestial poles
                args->getSource()->getMainMenu()->activateDialog("SlewDisplay");
                mount->startSlewingToTarget();
                break;
            }
        }
    }
}

void createGOMenu(MainMenu &mainMenu)
{
    for (unsigned int i = 0; i < sizeof(pointOfInterest) / sizeof(pointOfInterest[0]); i++)
    {
        goList.addMenuItem(new Button(pointOfInterest[i].pDisplay, goPointChosen));
    }
    goMenu.addMenuItem(&goList);
    mainMenu.addMenuItem(&goMenu);
}
