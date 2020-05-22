#include "Globals.h"
#include <EEPROM.h>

void EEPROMupdate(int loc, byte val)
{
  if (EEPROM.read(loc)!=val)
  {
    EEPROM.write(loc,val);  
  }
}
