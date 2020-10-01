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

// Using structs and list
//RAM:   [=====     ]  52.5% (used 4302 bytes from 8192 bytes)
//Flash: [==        ]  24.2% (used 63182 bytes from 261120 bytes)

// Using code (418 bytes saved!)
//RAM:   [=====     ]  47.4% (used 3884 bytes from 8192 bytes)
//Flash: [==        ]  24.7% (used 64496 bytes from 261120 bytes)

#define POI_Polaris 0
#define POI_VeilNebula 1
#define POI_M81BodesGalxy 2
#define POI_Cederblad214 3
#define POI_HeartNebula 4
#define POI_Navi 5
#define POI_SoulNebula 6
#define POI_ElephantTrunk 7
#define POI_BigDipper 8
#define POI_M101Pinwheel 9
#define POI_M51Whirlpool 10
#define POI_Deneb_Cygnus 11
#define POI_M63_Sunflower 12
#define POI_M31_Andromeda 13
#define POI_Vega 14
#define POI_Arcturus 15
#define POI_Altair 16
#define POI_M42_Orion_Nbula 17
#define POI_Lagoon_Nebula 18

// Add new items above here, not below. You MUST renumber the three items below accordingly
#define POI_Home 19
#define POI_Unpark 20
#define POI_Park 21

void goPointChosen(EventArgs *args)
{
    auto mount = Mount::instance();
    mount->stopSlewing(ALL_DIRECTIONS);
    int hemisphereOffset = -(NORTHERN_HEMISPHERE ? 90 : -90); // internal DEC degree is 0 at celestial poles
    bool startSlew = true;
    
    switch (((Button *)args->getSource())->getIndex())
    {
    case POI_Polaris:
        mount->targetRA().set(PolarisRAHour, PolarisRAMinute, PolarisRASecond);
        mount->targetDEC().set(89 + hemisphereOffset, 21, 6); 
        break;
    case POI_VeilNebula:
        mount->targetRA().set(20, 51, 28);
        mount->targetDEC().set(30 + hemisphereOffset, 59, 30);
        break;
    case POI_M81BodesGalxy:
        mount->targetRA().set(9, 57, 13);
        mount->targetDEC().set(68 + hemisphereOffset, 58, 1);
        break;
    case POI_Cederblad214:
        mount->targetRA().set(0, 5, 46);
        mount->targetDEC().set(67 + hemisphereOffset, 16, 45);
        break;
    case POI_HeartNebula:
        mount->targetRA().set(2, 34, 57);
        mount->targetDEC().set(61 + hemisphereOffset, 31, 17);
        break;
    case POI_Navi:
        mount->targetRA().set(0, 57, 57);
        mount->targetDEC().set(60 + hemisphereOffset, 49, 33);
        break;
    case POI_SoulNebula:
        mount->targetRA().set(2, 52, 47);
        mount->targetDEC().set(60 + hemisphereOffset, 30, 56);
        break;
    case POI_ElephantTrunk:
        mount->targetRA().set(21, 39, 44);
        mount->targetDEC().set(57 + hemisphereOffset, 35, 31);
        break;
    case POI_BigDipper:
        mount->targetRA().set(12, 16, 26);
        mount->targetDEC().set(56 + hemisphereOffset, 55, 7);
        break;
    case POI_M101Pinwheel:
        mount->targetRA().set(14, 3, 56);
        mount->targetDEC().set(54 + hemisphereOffset, 15, 0);
        break;
    case POI_M51Whirlpool:
        mount->targetRA().set(13, 30, 45);
        mount->targetDEC().set(47 + hemisphereOffset, 5, 21);
        break;
    case POI_Deneb_Cygnus:
        mount->targetRA().set(20, 42, 7);
        mount->targetDEC().set(45 + hemisphereOffset, 21, 12);
        break;
    case POI_M63_Sunflower:
        mount->targetRA().set(13, 16, 45);
        mount->targetDEC().set(41 + hemisphereOffset, 55, 14);
        break;
    case POI_M31_Andromeda:
        mount->targetRA().set(0, 43, 52);
        mount->targetDEC().set(41 + hemisphereOffset, 22, 53);
        break;
    case POI_Vega:
        mount->targetRA().set(18, 37, 37);
        mount->targetDEC().set(38 + hemisphereOffset, 48, 7);
        break;
    case POI_Arcturus:
        mount->targetRA().set(14, 16, 37);
        mount->targetDEC().set(19 + hemisphereOffset, 5, 21);
        break;
    case POI_Altair:
        mount->targetRA().set(19, 51, 45);
        mount->targetDEC().set(8 + hemisphereOffset, 55, 15);
        break;
    case POI_M42_Orion_Nbula:
        mount->targetRA().set(5, 36, 18);
        mount->targetDEC().set(-5 + hemisphereOffset, 22, 44);
        break;
    case POI_Lagoon_Nebula:
        mount->targetRA().set(18, 5, 2);
        mount->targetDEC().set(-24 + hemisphereOffset, 22, 52);
        break;

    case POI_Home:
        mount->goHome();
        startSlew = false;
        break;

    case POI_Unpark:
        mount->startSlewing(TRACKING);
        startSlew = false;
        break;

    case POI_Park:
        mount->park();
        startSlew = false;
        break;
    }

    if (startSlew)
    {
        args->getSource()->getMainMenu()->activateDialog("SlewDisplay");
        mount->startSlewingToTarget();
    }
}

void createGOMenu(MainMenu &mainMenu)
{
    goList.addMenuItem(new Button(F("Polaris"), goPointChosen, POI_Polaris));
    goList.addMenuItem(new Button(F("Veil Nebula"), goPointChosen, POI_VeilNebula));
    goList.addMenuItem(new Button(F("M81 Bodes Galxy"), goPointChosen, POI_M81BodesGalxy));
    goList.addMenuItem(new Button(F("Cederblad 214"), goPointChosen, POI_Cederblad214));
    goList.addMenuItem(new Button(F("Heart Nebula"), goPointChosen, POI_HeartNebula));
    goList.addMenuItem(new Button(F("Navi"), goPointChosen, POI_Navi));
    goList.addMenuItem(new Button(F("Soul Nebula"), goPointChosen, POI_SoulNebula));
    goList.addMenuItem(new Button(F("Elephant Trunk"), goPointChosen, POI_ElephantTrunk));
    goList.addMenuItem(new Button(F("Big Dipper"), goPointChosen, POI_BigDipper));
    goList.addMenuItem(new Button(F("M101 Pinwheel"), goPointChosen, POI_M101Pinwheel));
    goList.addMenuItem(new Button(F("M51 Whirlpool"), goPointChosen, POI_M51Whirlpool));
    goList.addMenuItem(new Button(F("Deneb (Cygnus)"), goPointChosen, POI_Deneb_Cygnus));
    goList.addMenuItem(new Button(F("M63 Sunflower"), goPointChosen, POI_M63_Sunflower));
    goList.addMenuItem(new Button(F("M31 Andromeda"), goPointChosen, POI_M31_Andromeda));
    goList.addMenuItem(new Button(F("Vega"), goPointChosen, POI_Vega));
    goList.addMenuItem(new Button(F("Arcturus"), goPointChosen, POI_Arcturus));
    goList.addMenuItem(new Button(F("Altair"), goPointChosen, POI_Altair));
    goList.addMenuItem(new Button(F("M42 Orion Nbula"), goPointChosen, POI_M42_Orion_Nbula));
    goList.addMenuItem(new Button(F("Lagoon Nebula"), goPointChosen, POI_Lagoon_Nebula));
    goList.addMenuItem(new Button(F("Home"), goPointChosen, POI_Home));
    goList.addMenuItem(new Button(F("Unpark"), goPointChosen, POI_Unpark));
    goList.addMenuItem(new Button(F("Park"), goPointChosen, POI_Park));

    goMenu.addMenuItem(&goList);
    mainMenu.addMenuItem(&goMenu);
}
