#include <EEPROM.h>
#include "EPROMStore.hpp"
#include "Utility.hpp"

// The platform-independant EEPROM class

#ifdef ESPBOARD

// Initialize the EEPROM object for ESP boards, settign aside 32 bytes for storage
void EPROMStore::initialize()
{
  LOGV1(DEBUG_VERBOSE, F("EEPROM[ESP]: Startup with 32 bytes"));
  EEPROM.begin(32);
}

// Update the given location with the given value
void EPROMStore::update(int location, uint8_t value)
{
  LOGV3(DEBUG_VERBOSE, F("EEPROM[ESP]: Writing %x to %d"), value, location);
  EEPROM.write(location, value);
  LOGV1(DEBUG_VERBOSE, F("EEPROM[ESP]: Committing"));
  EEPROM.commit();
}

// Read the value at the given location
uint8_t EPROMStore::read(int location)
{
  uint8_t value;
  value = EEPROM.read(location);
  LOGV3(DEBUG_VERBOSE, F("EEPROM[ESP]: Read %x from %d"), value, location);
  return value;
}

#else

// Initialize the EEPROM storage in a platform-independent abstraction
void EPROMStore::initialize()
{
  LOGV1(DEBUG_VERBOSE, F("EEPROM[UNO]: Startup "));
}

// Update the given location with the given value
void EPROMStore::update(int location, uint8_t value)
{
  //LOGV3(DEBUG_VERBOSE, F("EEPROM[UNO]: Writing8 %x to %d"), value, location);
  EEPROM.write(location, value);
}

// Read the value at the given location
uint8_t EPROMStore::read(int location)
{
  uint8_t value = EEPROM.read(location);
  //LOGV3(DEBUG_VERBOSE, F("EEPROM[UNO]: Read8 %x from %d"), value, location);
  return value;
}

#endif

void EPROMStore::updateInt16(int loByteAddr, int hiByteAddr, int16_t value)
{
  //LOGV4(DEBUG_VERBOSE, F("EEPROM: Writing16 %d to %d, %d"), value, loByteAddr, hiByteAddr);
  update(loByteAddr, value & 0x00FF);
  update(hiByteAddr, (value >> 8) & 0x00FF);
}

int16_t EPROMStore::readInt16(int loByteAddr, int hiByteAddr)
{
  uint8_t valLo = EPROMStore::read(loByteAddr);
  uint8_t valHi = EPROMStore::read(hiByteAddr);
  uint16_t uValue = (uint16_t)valLo + (uint16_t)valHi * 256;
  int16_t value = static_cast<int16_t>(uValue);
  //LOGV4(DEBUG_VERBOSE, F("EEPROM: Read16 %d from %d, %d"), value, loByteAddr, hiByteAddr);
  return value;
}

void EPROMStore::updateInt32(int lowestByteAddr, int32_t value)
{
  LOGV4(DEBUG_VERBOSE, F("EEPROM: Writing32 %l to %d-%d"), value, lowestByteAddr, lowestByteAddr + 3);
  update(lowestByteAddr, value & 0x00FF);
  update(lowestByteAddr + 1, (value >> 8) & 0x00FF);
  update(lowestByteAddr + 2, (value >> 16) & 0x00FF);
  update(lowestByteAddr + 3, (value >> 24) & 0x00FF);
}

int32_t EPROMStore::readInt32(int lowestByteAddr)
{
  uint8_t val1 = EPROMStore::read(lowestByteAddr);
  uint8_t val2 = EPROMStore::read(lowestByteAddr + 1);
  uint8_t val3 = EPROMStore::read(lowestByteAddr + 2);
  uint8_t val4 = EPROMStore::read(lowestByteAddr + 3);
  LOGV5(DEBUG_INFO, F("EEPROM: Read32 read these bytes: %x %x %x %x"), val1, val2, val3, val4);
  uint32_t uValue = (uint32_t)val1 + (uint32_t)val2 * 256 + (uint32_t)val3 * 256 * 256 + (uint32_t)val4 * 256 * 256 * 256;
  int32_t value = static_cast<int32_t>(uValue);
  LOGV4(DEBUG_INFO, F("EEPROM: Read32 which is %l from %d-%d"), value, lowestByteAddr, lowestByteAddr + 3);
  return value;
}
