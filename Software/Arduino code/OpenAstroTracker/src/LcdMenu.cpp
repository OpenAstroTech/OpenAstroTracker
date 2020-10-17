#include "Utility.hpp"
#include "EPROMStore.hpp"
#include "LcdMenu.hpp"

#if DISPLAY_TYPE > 0

// Class that drives the LCD screen with a menu
// You add a string and an id item and this class handles the display and navigation
// Create a new menu, using the given number of LCD display columns and rows
#if DISPLAY_TYPE == DISPLAY_TYPE_LCD_KEYPAD
LcdMenu::LcdMenu(byte cols, byte rows, int maxItems) : _lcd(8, 9, 4, 5, 6, 7)
{
  _cols = cols;
  _rows = rows;
  _maxItems = maxItems;
}
#elif DISPLAY_TYPE == DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23008 || DISPLAY_TYPE == DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23017
LcdMenu::LcdMenu(byte cols, byte rows, int maxItems) : _lcd(0x20)
{
  #if DISPLAY_TYPE == DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23017
  _lcd.setMCPType(LTI_TYPE_MCP23017);
  #elif DISPLAY_TYPE == DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23008
  _lcd.setMCPType(LTI_TYPE_MCP23008);
  #endif

  _cols = cols;
  _rows = rows;
  _maxItems = maxItems;
}
#endif

void LcdMenu::startup()
{
  LOGV1(DEBUG_INFO, F("LcdMenu:: startup"));

  _lcd.begin(_cols, _rows);
  _numMenuItems = 0;
  _activeMenuIndex = 0;
  _longestDisplay = 0;
  _columns = _cols;
  _activeRow = -1;
  _activeCol = -1;
  _lastDisplay[0] = "";
  _lastDisplay[1] = "";
  _menuItems = new MenuItem *[_maxItems];
  
  #if DISPLAY_TYPE == DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23008 || DISPLAY_TYPE == DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23017
  _lcd.setBacklight(RED);
  #endif

  _brightness = EPROMStore::read(16);
  LOGV2(DEBUG_INFO, F("LCD: Brightness from EEPROM is %d"), _brightness);
  // pinMode(10, OUTPUT);
  // analogWrite(10, _brightness);

  // Create special characters for degrees and arrows
  _lcd.createChar(_degrees, DegreesBitmap);
  _lcd.createChar(_minutes, MinutesBitmap);
  _lcd.createChar(_leftArrow, LeftArrowBitmap);
  _lcd.createChar(_rightArrow, RightArrowBitmap);
  _lcd.createChar(_upArrow, UpArrowBitmap);
  _lcd.createChar(_downArrow, DownArrowBitmap);
  _lcd.createChar(_tracking, TrackingBitmap);
  _lcd.createChar(_noTracking, NoTrackingBitmap);
}

// Find a menu item by its ID
MenuItem *LcdMenu::findById(byte id)
{
  for (byte i = 0; i < _numMenuItems; i++)
  {
    if (_menuItems[i]->id() == id)
    {
      return _menuItems[i];
    }
  }
  return NULL;
}

// Add a new menu item to the list (order matters)
void LcdMenu::addItem(const char *disp, byte id)
{
  _menuItems[_numMenuItems++] = new MenuItem(disp, id);
  _longestDisplay = max(_longestDisplay, strlen(disp));
}

// Get the currently active item ID
byte LcdMenu::getActive()
{
  return _menuItems[_activeMenuIndex]->id();
}

// Set the active menu item
void LcdMenu::setActive(byte id)
{
  for (byte i = 0; i < _numMenuItems; i++)
  {
    if (_menuItems[i]->id() == id)
    {
      _activeMenuIndex = i;
      break;
    }
  }
}

// Pass thru utility function
void LcdMenu::setCursor(byte col, byte row)
{
  _activeRow = row;
  _activeCol = col;
}

// Pass thru utility function
void LcdMenu::clear()
{
  _lcd.clear();
}

// Set the brightness of the backlight
void LcdMenu::setBacklightBrightness(int level, bool persist)
{
  _brightness = level;

  //LOGV2(DEBUG_INFO, F("LCD: Writing %d as brightness"), _brightness  );
  // analogWrite(10, _brightness);
  //LOGV2(DEBUG_INFO, F("LCD: Wrote %d as brightness"), _brightness  );

  if (persist)
  {
    // LOGV2(DEBUG_INFO, F("LCD: Saving %d as brightness"), (_brightness & 0x00FF));
    EPROMStore::update(16, (byte)(_brightness & 0x00FF));
  }
}

// Get the current brightness
int LcdMenu::getBacklightBrightness()
{
  return _brightness;
}

// Go to the next menu item from currently active one
void LcdMenu::setNextActive()
{

  _activeMenuIndex = adjustWrap(_activeMenuIndex, 1, 0, _numMenuItems - 1);

  // Update the display
  updateDisplay();

  // Clear submenu line, in case new menu doesn't print anything.
  _lcd.setCursor(0, 1);
  for (byte i = 0; i < _columns; i++)
  {
    _lcd.print(" ");
  }
}

// Update the display of the LCD with the current menu settings
// This iterates over the menu items, building a menu string by concatenating their display string.
// It also places the selector arrows around the active one.
// It then sends the string to the LCD, keeping the selector arrows centered in the same place.
void LcdMenu::updateDisplay()
{

  char bufMenu[17];
  char *pBufMenu = &bufMenu[0];
  String menuString = "";
  byte offsetToActive = 0;
  byte offset = 0;

  char scratchBuffer[12];
  // Build the entire menu string
  for (byte i = 0; i < _numMenuItems; i++)
  {
    MenuItem *item = _menuItems[i];
    bool isActive = i == _activeMenuIndex;
    sprintf(scratchBuffer, "%c%s%c", isActive ? '>' : ' ', item->display(), isActive ? '<' : ' ');

    // For the active item remember where it starts in the string and insert selector arrows
    offsetToActive = isActive ? offset : offsetToActive;
    menuString += String(scratchBuffer);
    offset += strlen(scratchBuffer);
  }

  _lcd.setCursor(0, 0);
  _activeRow = 0;
  _activeCol = 0;
  int usableColumns = _columns - 1; // Leave off last one to have distance to tracking indicator

  // Determine where to place the active menu item. (empty space around longest item divided by two).
  int margin = (usableColumns - (_longestDisplay)) / 2;
  int offsetIntoString = offsetToActive - margin;

  // Pad the front if we don't have enough to offset the string to the arrow locations (happens on first item(s))
  while (offsetIntoString < 0)
  {
    *(pBufMenu++) = ' ';
    offsetIntoString++;
  }

  // Display the actual menu string
  while ((pBufMenu < bufMenu + usableColumns) && (offsetIntoString < (int)menuString.length()))
  {
    *(pBufMenu++) = menuString[offsetIntoString++];
  }

  // Pad the end with spaces so the display is cleared when getting to the last item(s).
  while (pBufMenu < bufMenu + _columns)
  {
    *(pBufMenu++) = ' ';
  }
  *(pBufMenu++) = 0;

  printMenu(String(bufMenu));

  setCursor(0, 1);
}

// Print the given character to the LCD, converting some special ones to our bitmaps
void LcdMenu::printChar(char ch)
{
  if (ch == '>')
  {
    _lcd.write(_rightArrow);
  }
  else if (ch == '<')
  {
    _lcd.write(_leftArrow);
  }
  else if (ch == '^')
  {
    _lcd.write(_upArrow);
  }
  else if (ch == '~')
  {
    _lcd.write(_downArrow);
  }
  else if (ch == '@')
  {
    _lcd.write(_degrees);
  }
  else if (ch == '\'')
  {
    _lcd.write(_minutes);
  }
  else if (ch == '&')
  {
    _lcd.write(_tracking);
  }
  else if (ch == '`')
  {
    _lcd.write(_noTracking);
  }
  else
  {
    _lcd.print(ch);
  }
}

// Print a character at a specific position
void LcdMenu::printAt(int col, int row, char ch)
{
  _lcd.setCursor(col, row);
  printChar(ch);
}

#if DISPLAY_TYPE == DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23008 || DISPLAY_TYPE == DISPLAY_TYPE_LCD_KEYPAD_I2C_MCP23017
uint8_t LcdMenu::readButtons()
{
  return _lcd.readButtons();
}
#endif

// Print a string to the LCD at the current cursor position, substituting the special arrows and padding with spaces to the end
void LcdMenu::printMenu(String line)
{
  if ((_lastDisplay[_activeRow] != line) || (_activeCol != 0))
  {

    _lastDisplay[_activeRow] = line;

    _lcd.setCursor(_activeCol, _activeRow);
    int spaces = _columns - line.length();
    for (unsigned int i = 0; i < line.length(); i++)
    {
      printChar(line[i]);
    }

    // Clear the rest of the display
    while (spaces > 0)
    {
      _lcd.print(" ");
      spaces--;
    }
  }
}

// The right arrow bitmap
byte LcdMenu::RightArrowBitmap[8] = {
    B00000,
    B01000,
    B01100,
    B01110,
    B01100,
    B01000,
    B00000,
    B00000};

// The left arrow bitmap
byte LcdMenu::LeftArrowBitmap[8] = {
    B00000,
    B00010,
    B00110,
    B01110,
    B00110,
    B00010,
    B00000,
    B00000};

byte LcdMenu::UpArrowBitmap[8] = {
    B00100,
    B01110,
    B11111,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100};

byte LcdMenu::DownArrowBitmap[8] = {
    B000100,
    B000100,
    B000100,
    B000100,
    B000100,
    B011111,
    B001110,
    B000100};

byte LcdMenu::DegreesBitmap[8] = {
    B01100,
    B10010,
    B10010,
    B01100,
    B00000,
    B00000,
    B00000,
    B00000};

byte LcdMenu::MinutesBitmap[8] = {
    B01000,
    B01000,
    B01000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000};

byte LcdMenu::TrackingBitmap[8] = {
    B10111,
    B00010,
    B10010,
    B00010,
    B10111,
    B00101,
    B10110,
    B00101};

byte LcdMenu::NoTrackingBitmap[8] = {
    B10000,
    B00000,
    B10000,
    B00010,
    B10000,
    B00000,
    B10000,
    B00000};

#else

LcdMenu::LcdMenu(byte cols, byte rows, int maxItems)
{
}

MenuItem *LcdMenu::findById(byte id)
{
  return NULL;
}

void LcdMenu::addItem(const char *disp, byte id) {}

byte LcdMenu::getActive()
{
  return 0;
}

void LcdMenu::setActive(byte id) {}

void LcdMenu::setCursor(byte col, byte row) {}

void LcdMenu::clear() {}

void LcdMenu::setNextActive() {}

void LcdMenu::updateDisplay() {}

void LcdMenu::printMenu(String line) {}

void LcdMenu::printChar(char ch) {}

void LcdMenu::printAt(int col, int row, char ch) {}

#endif
