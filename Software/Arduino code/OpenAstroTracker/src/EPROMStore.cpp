#include <EEPROM.h>
#include "EPROMStore.hpp"
#include "Utility.hpp"

// The platform-independant EEPROM class

#if USE_DUMMY_EEPROM == 1

static uint8_t dummyEepromStorage[EPROMStore::STORE_SIZE];

// Initialize the EEPROM object for ESP boards, setting aside storage
void EPROMStore::initialize()
{
  LOGV2(DEBUG_EEPROM, F("EEPROM[DUMMY]: Startup with %d bytes"), EPROMStore::STORE_SIZE);
  memset(dummyEepromStorage, 0, sizeof(dummyEepromStorage));
}

// Update the given location with the given value
void EPROMStore::update(uint8_t location, uint8_t value)
{
  LOGV3(DEBUG_EEPROM, F("EEPROM[DUMMY]: Writing %x to %d"), value, location);
  dummyEepromStorage[location] = value;
}

// Read the value at the given location
uint8_t EPROMStore::read(uint8_t location)
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
  LOGV2(DEBUG_EEPROM, F("EEPROM[ESP]: Startup with %d bytes"), EPROMStore::STORE_SIZE);
  EEPROM.begin(EPROMStore::STORE_SIZE);
}

// Update the given location with the given value
void EPROMStore::update(uint8_t location, uint8_t value)
{
  LOGV3(DEBUG_EEPROM, F("EEPROM[ESP]: Writing %x to %d"), value, location);
  EEPROM.write(location, value);
  LOGV1(DEBUG_EEPROM, F("EEPROM[ESP]: Committing"));
  EEPROM.commit();
}

// Read the value at the given location
uint8_t EPROMStore::read(uint8_t location)
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
void EPROMStore::update(uint8_t location, uint8_t value)
{
  LOGV3(DEBUG_EEPROM, F("EEPROM[Uno/Mega]: Writing8 %x to %d"), value, location);
  EEPROM.write(location, value);
}

// Read the value at the given location
uint8_t EPROMStore::read(uint8_t location)
{
  uint8_t value = EEPROM.read(location);
  LOGV3(DEBUG_EEPROM, F("EEPROM[Uno/Mega]: Read8 %x from %d"), value, location);
  return value;
}

#endif

// Update the given location with the given value
void EPROMStore::updateUint8(EPROMStore::Key location, uint8_t value)
{
  LOGV4(DEBUG_EEPROM, F("EEPROM: Writing8 %x (%d) to %d"), value, value, location);
  update(location, value);
}

// Read the value at the given location
uint8_t EPROMStore::readUint8(EPROMStore::Key location)
{
  uint8_t value;
  value = read(location);
  LOGV4(DEBUG_EEPROM, F("EEPROM: Read8 %x (%d) from %d"), value, value, location);
  return value;
}

void EPROMStore::updateInt16(EPROMStore::Key location, int16_t value)
{
  LOGV4(DEBUG_EEPROM, F("EEPROM: Writing16 %x (%d) to %d"), value, value, location);
  update(location, value & 0x00FF);
  update(location+1, (value >> 8) & 0x00FF);
}

int16_t EPROMStore::readInt16(EPROMStore::Key location)
{
  uint8_t valLo = EPROMStore::read(location);
  uint8_t valHi = EPROMStore::read(location+1);
  uint16_t uValue = (uint16_t)valLo + (uint16_t)valHi * 256;
  int16_t value = static_cast<int16_t>(uValue);
  LOGV4(DEBUG_EEPROM, F("EEPROM: Read16 %x (%d) from %d"), value, value, location);
  return value;
}

void EPROMStore::updateUint16(EPROMStore::Key location, uint16_t value)
{
  LOGV4(DEBUG_EEPROM, F("EEPROM: Writing16 %x (%d) to %d"), value, value, location);
  update(location, value & 0x00FF);
  update(location+1, (value >> 8) & 0x00FF);
}

uint16_t EPROMStore::readUint16(EPROMStore::Key location)
{
  uint8_t valLo = EPROMStore::read(location);
  uint8_t valHi = EPROMStore::read(location+1);
  uint16_t value = (uint16_t)valLo + (uint16_t)valHi * 256;
  LOGV4(DEBUG_EEPROM, F("EEPROM: Read16 %x (%d) from %d"), value, value, location);
  return value;
}

void EPROMStore::updateInt32(EPROMStore::Key location, int32_t value)
{
  LOGV4(DEBUG_EEPROM, F("EEPROM: Writing32 %x (%l) to %d"), value, value, location);
  update(location, value & 0x00FF);
  update(location + 1, (value >> 8) & 0x00FF);
  update(location + 2, (value >> 16) & 0x00FF);
  update(location + 3, (value >> 24) & 0x00FF);
}

int32_t EPROMStore::readInt32(EPROMStore::Key location)
{
  uint8_t val1 = EPROMStore::read(location);
  uint8_t val2 = EPROMStore::read(location + 1);
  uint8_t val3 = EPROMStore::read(location + 2);
  uint8_t val4 = EPROMStore::read(location + 3);
  LOGV5(DEBUG_EEPROM, F("EEPROM: Read32 read these bytes: %x %x %x %x"), val1, val2, val3, val4);
  uint32_t uValue = (uint32_t)val1 + (uint32_t)val2 * 256 + (uint32_t)val3 * 256 * 256 + (uint32_t)val4 * 256 * 256 * 256;
  int32_t value = static_cast<int32_t>(uValue);
  LOGV3(DEBUG_EEPROM, F("EEPROM: Read32 which is %l from %d"), value, location);
  return value;
}
