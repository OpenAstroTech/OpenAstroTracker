// A single menu item (like RA, HEAT, POL, etc.)
// The ID is just a number, it has no relevance for the order of the items
// The display is what is shown on the menu.
class MenuItem {
    const char* _display;     // What to display on the screen
    byte _id;             // The ID of the menu item
  public:
    MenuItem(const char *display, byte id) {
      _display = display;
      _id = id;
    }

    const char*  display() {
      return _display;
    }

    byte id() {
      return _id;
    }
};

// Class that drives the LCD screen with a menu
// You add a string and an id item and this class handles the display and navigation
class LcdMenu {
  private:
    LiquidCrystal _lcd;   // The LCD screen that we'll display the menu on
    MenuItem* _menuItems[MAXMENUITEMS];  // The first menu item (linked list)
    byte _numMenuItems;
    byte _activeMenuIndex;
    byte _activeId;         // The id of the currently active menu item
    byte _longestDisplay;   // The number of characters in the longest menu item
    byte _columns;          // The number of columns in the LCD display

    byte _degrees = 1;
    byte _minutes = 2;
    byte _leftArrow = 3;
    byte _rightArrow = 4;
    byte _upArrow = 5;
    byte _downArrow = 6;

  public:
    // Create a new menu, using the given number of LCD display columns and rows
    LcdMenu(byte cols, byte rows) : _lcd(8, 9, 4, 5, 6, 7) {
      //_lcd = new LiquidCrystal(8, 9, 4, 5, 6, 7);
      _lcd.begin(cols, rows);

      _activeId = 0;
      _numMenuItems = 0;
      _activeMenuIndex = 0;
      _longestDisplay = 0;
      _columns = cols;

      // Create special characters for degrees and arrows
      _lcd.createChar(_degrees, DegreesBitmap);
      _lcd.createChar(_minutes, MinutesBitmap);
      _lcd.createChar(_leftArrow, LeftArrowBitmap);
      _lcd.createChar(_rightArrow, RightArrowBitmap);
      _lcd.createChar(_upArrow, UpArrowBitmap);
      _lcd.createChar(_downArrow, DownArrowBitmap);
    }

    // Find a menu item by its ID
    MenuItem* findById(byte id)
    {
      for (byte i = 0; i < _numMenuItems; i++) {
        if (_menuItems[i]->id() == id) {
          return _menuItems[i];
        }
      }
      return NULL;
    }

    // Add a new menu item to the list (order matters)
    void addItem(const char *disp, byte id) {
      _menuItems[_numMenuItems++] = new MenuItem(disp, id);
      _longestDisplay = max(_longestDisplay, strlen(disp));
    }

    // Get the currently active item ID
    byte getActive() {
      return _activeId;
    }

    // Set the active menu item
    void setActive(byte id) {
      _activeId = id;
      for (byte i = 0; i < _numMenuItems; i++) {
        if (_menuItems[i]->id() == id) {
          _activeMenuIndex = i;
          break;
        }
      }
    }

    // Pass thru utility function
    void setCursor(byte col, byte row) {
      _lcd.setCursor(col, row);
    }

    // Pass thru utility function
    void clear() {
      _lcd.clear();
    }

    // Go to the next menu item from currently active one
    void setNextActive() {
      _activeMenuIndex = adjustWrap(_activeMenuIndex, 1, 0, _numMenuItems - 1);
      _activeId = _menuItems[_activeMenuIndex]->id();

      // Update the display
      _lcd.setCursor(0, 0);
      updateDisplay();

      // Clear submenu line, in case new menu doesn't print anything.
      _lcd.setCursor(0, 1);
      for (byte i = 0; i < _columns; i++) {
        _lcd.print(" ");
      }
    }

    // Update the display of the LCD with the current menu settings
    // This iterates over the menu items, building a menu string by concatenating their display string.
    // It also places the selector arrows around the active one.
    // It then sends the string to the LCD, keeping the selector arrows centered in the same place.
    void updateDisplay() {
      _lcd.setCursor(0, 0);
      String menuString = "";
      byte offsetToActive = 0;
      byte offset = 0;

      // Build the entire menu string
      for (byte i = 0; i < _numMenuItems; i++) {
        MenuItem* item = _menuItems[i];
        bool isActive = item->id() == _activeId;
        sprintf(scratchBuffer, "%c%s%c", isActive ? GREATER_THAN : 0x20, item->display(), isActive ? LESS_THAN : 0x20);
       
        // For the active item remember where it starts in the string and insert selector arrows
        offsetToActive = isActive ? offset : offsetToActive ;
        menuString += String(scratchBuffer);
        offset += strlen(scratchBuffer);
      }

      // Determine where to place the active menu item. (empty space around longest item divided by two).
      byte margin = (_columns - (_longestDisplay)) / 2;
      int offsetIntoString = offsetToActive - margin;

      // Pad the front if we don't have enough to offset the string to the arrow locations (happens on first item(s))
      while (offsetIntoString < 0) {
        menuString = " " + menuString;
        offsetIntoString++;
      }

      // Display the actual menu string
      String displayString = menuString.substring(offsetIntoString, offsetIntoString + _columns);

      // Pad the end with spaces so the display is cleared when getting to the last item(s).
      while (displayString.length() < _columns) {
        displayString += " ";
      }

      printMenu(displayString);
    }

    void printChar(char ch) {
      if (ch == '>') {
        _lcd.write(_rightArrow);
      } else if (ch == '<') {
        _lcd.write(_leftArrow);
      } else if (ch == '^') {
        _lcd.write(_upArrow);
      } else if (ch == '~') {
        _lcd.write(_downArrow);
      } else if (ch == '@') {
        _lcd.write(_degrees);
      } else if (ch == '\'') {
        _lcd.write(_minutes);
      } else {
        _lcd.print(ch);
      }
    }
    
    // Print a string to the LCD at the current cursor position, substituting the special arrows and padding with spaces to the end
    void printMenu(String line)
    {
      int spaces = _columns - line.length();
      for (int i = 0; i < line.length(); i++) {
        printChar(line[i]);
      }

      // Clear the rest of the display
      while (spaces > 0) {
        _lcd.print(" ");
        spaces--;
      }
    }

    // The right arrow bitmap
    byte RightArrowBitmap[8] = {
      B00000,
      B01000,
      B01100,
      B01110,
      B01100,
      B01000,
      B00000,
      B00000
    };

    // The left arrow bitmap
    byte LeftArrowBitmap[8] = {
      B00000,
      B00010,
      B00110,
      B01110,
      B00110,
      B00010,
      B00000,
      B00000
    };

    byte UpArrowBitmap[8] = {
      B00100,
      B01110,
      B11111,
      B00100,
      B00100,
      B00100,
      B00100,
      B00100
    };

    byte DownArrowBitmap[8] = {
      B000100,
      B000100,
      B000100,
      B000100,
      B000100,
      B011111,
      B001110,
      B000100
    };

    byte DegreesBitmap[8] = {
      B01100,
      B10010,
      B10010,
      B01100,
      B00000,
      B00000,
      B00000,
      B00000
    };

    byte MinutesBitmap[8] = {
      B01000,
      B01000,
      B01000,
      B00000,
      B00000,
      B00000,
      B00000,
      B00000
    };
};
