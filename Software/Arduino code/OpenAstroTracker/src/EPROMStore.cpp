#include <EEPROM.h>
#include "EPROMStore.hpp"
#include "Utility.hpp"

// The platform-independant EEPROM class

#if USE_DUMMY_EEPROM == 1

static uint8_t dummyEepromStorage[32];

// Initialize the EEPROM object for ESP boards, settign aside 32 bytes for storage
void EPROMStore::initialize()
{
  LOGV1(DEBUG_EEPROM, F("EEPROM[DUMMY]: Startup with 32 bytes"));
  memset(dummyEepromStorage, sizeof(dummyEepromStorage), 1);
}

// Update the given location with the given value
void EPROMStore::update(int location, uint8_t value)
{
  LOGV3(DEBUG_EEPROM, F("EEPROM[DUMMY]: Writing %x to %d"), value, location);
  dummyEepromStorage[location] = value;
}

// Read the value at the given location
uint8_t EPROMStore::read(int location)
{
  uint8_t value;
  value = dummyEepromStorage[location];
  LOGV3(DEBUG_EEPROM, F("EEPROM[DUMMY]: Read %x from %d"), value, location);
  return value;
}

#elif defined(ESPBOARD)

// Initialize the EEPROM object for ESP boards, settign aside 32 bytes for storage
void EPROMStore::initialize()
{
  LOGV1(DEBUG_EEPROM, F("EEPROM[ESP]: Startup with 32 bytes"));
  EEPROM.begin(32);
}

// Update the given location with the given value
void EPROMStore::update(int location, uint8_t value)
{
  LOGV3(DEBUG_EEPROM, F("EEPROM[ESP]: Writing %x to %d"), value, location);
  EEPROM.write(location, value);
  LOGV1(DEBUG_EEPROM, F("EEPROM[ESP]: Committing"));
  EEPROM.commit();
}

// Read the value at the given location
uint8_t EPROMStore::read(int location)
{
  uint8_t value;
  value = EEPROM.read(location);
  LOGV3(DEBUG_EEPROM, F("EEPROM[ESP]: Read %x from %d"), value, location);
  return value;
}

#else

// Initialize the EEPROM storage in a platform-independent abstraction
void EPROMStore::initialize()
{
  LOGV1(DEBUG_EEPROM, F("EEPROM[Uno/Mega]: Startup "));
}

// Update the given location with the given value
void EPROMStore::update(int location, uint8_t value)
{
  LOGV3(DEBUG_EEPROM, F("EEPROM[Uno/Mega]: Writing8 %x to %d"), value, location);
  EEPROM.write(location, value);
}

// Read the value at the given location
uint8_t EPROMStore::read(int location)
{
  uint8_t value = EEPROM.read(location);
  LOGV3(DEBUG_EEPROM, F("EEPROM[Uno/Mega]: Read8 %x from %d"), value, location);
  return value;
}

#endif

void EPROMStore::updateInt16(int loByteAddr, int hiByteAddr, int16_t value)
{
  LOGV5(DEBUG_EEPROM, F("EEPROM: Writing16 %x (%d) to %d, %d"), value, value, loByteAddr, hiByteAddr);
  update(loByteAddr, value & 0x00FF);
  update(hiByteAddr, (value >> 8) & 0x00FF);
}

int16_t EPROMStore::readInt16(int loByteAddr, int hiByteAddr)
{
  uint8_t valLo = EPROMStore::read(loByteAddr);
  uint8_t valHi = EPROMStore::read(hiByteAddr);
  uint16_t uValue = (uint16_t)valLo + (uint16_t)valHi * 256;
  int16_t value = static_cast<int16_t>(uValue);
  LOGV5(DEBUG_EEPROM, F("EEPROM: Read16 %x (%d) from %d, %d"), value, value, loByteAddr, hiByteAddr);
  return value;
}

void EPROMStore::updateInt32(int lowestByteAddr, int32_t value)
{
  LOGV5(DEBUG_EEPROM, F("EEPROM: Writing32 %x (%l) to %d-%d"), value, value, lowestByteAddr, lowestByteAddr + 3);
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
  LOGV5(DEBUG_EEPROM, F("EEPROM: Read32 read these bytes: %x %x %x %x"), val1, val2, val3, val4);
  uint32_t uValue = (uint32_t)val1 + (uint32_t)val2 * 256 + (uint32_t)val3 * 256 * 256 + (uint32_t)val4 * 256 * 256 * 256;
  int32_t value = static_cast<int32_t>(uValue);
  LOGV4(DEBUG_EEPROM, F("EEPROM: Read32 which is %l from %d-%d"), value, lowestByteAddr, lowestByteAddr + 3);
  return value;
}
