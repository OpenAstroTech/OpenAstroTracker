#pragma once
#include <WString.h>
#include "Mount.hpp"
#include "LcdMenu.hpp"

class MeadeCommandProcessor
{
public: 
    String processCommand(String incmd);
    MeadeCommandProcessor(Mount* mount, LcdMenu* lcdMenu);


private: 
    String handleMeadeSetInfo(String inCmd);
    String handleMeadeMovement(String inCmd);
    String handleMeadeGetInfo(String inCmd);
    String handleMeadeSyncControl(String inCmd);
    String handleMeadeHome(String inCmd);
    String handleMeadeInit(String inCmd);
    String handleMeadeQuit(String inCmd);
    String handleMeadeSetSlewRate(String inCmd);
    Mount* _mount;
    LcdMenu* _lcdMenu;
};