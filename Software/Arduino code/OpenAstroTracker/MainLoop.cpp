#include <Arduino.h>
#include "lib/input/LcdButtons.hpp"
#include "lib/menu/controls/MainMenu.hpp"
#include "Configuration.hpp"
#include "Utility.hpp"
#include "OatMenu.hpp"
#include "Mount.hpp"
#include "MeadeCommandProcessor.hpp"

extern MainMenu mainMenu;
void serialLoop();

bool quitSerialOnNextButtonRelease  = false;

void loop()
{
  delay(20);
  //LOGV1(DEBUG_ANY, "main loop start");

  Mount::instance()->loop();

#if SUPPORT_SERIAL_CONTROL == 1
  if (inSerialControl)
  {
    byte lcd_key;
    if (LcdButtons::instance()->keyChanged(&lcd_key))
    {
      if (lcd_key == btnSELECT)
      {
        quitSerialOnNextButtonRelease = true;
      }
      else if ((lcd_key == btnNONE) && quitSerialOnNextButtonRelease)
      {
        MeadeCommandProcessor::instance()->processCommand(":Qq#");
        quitSerialOnNextButtonRelease = false;
      }
    }
    serialLoop();
  }
  else
#endif
  {
    runMenuSystem(mainMenu);
  }

  //LOGV1(DEBUG_ANY, "main loop end");
}