#pragma once
#include <WString.h>
#include "Mount.hpp"
#include "lib/output/LcdDisplay.hpp"

class MeadeCommandProcessor
{
public:
  static MeadeCommandProcessor* createProcessor(Mount* mount, LcdDisplay* lcdDisplay);
  static MeadeCommandProcessor* instance();
  String processCommand(String incmd);

private:
  MeadeCommandProcessor(Mount* mount, LcdDisplay* lcdDisplay);
  String handleMeadeSetInfo(String inCmd);
  String handleMeadeMovement(String inCmd);
  String handleMeadeGetInfo(String inCmd);
  String handleMeadeSyncControl(String inCmd);
  String handleMeadeHome(String inCmd);
  String handleMeadeInit(String inCmd);
  String handleMeadeQuit(String inCmd);
  String handleMeadeDistance(String inCmd);
  String handleMeadeSetSlewRate(String inCmd);
  String handleMeadeExtraCommands(String inCmd);
  Mount* _mount;
  LcdDisplay* _lcdDisplay;
  static MeadeCommandProcessor* _instance;
};
