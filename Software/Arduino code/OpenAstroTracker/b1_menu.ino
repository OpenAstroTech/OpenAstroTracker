// A single menu item (like RA, HEAT, POL, etc.)
// The ID is just a number, it has no relevance for the order of the items
// The display is what is shown on the menu.
class MenuItem {
    String _display;     // What to display on the screen
    int _id;             // The ID of the menu item
    MenuItem* _nextItem; // The next menu item (linked list)
  public:
    MenuItem(String display, int id) {
      _display = display;
      _id = id;
      _nextItem = NULL;
    }

    String display() {
      return _display;
    }

    int id() {
      return _id;
    }

    MenuItem* nextItem() {
      return _nextItem;
    }

    void setNextItem(MenuItem* next) {
      _nextItem = next;
    }
};

// Class that drives the LCD screen with a menu
// You add a string and an id item and this class handles the display and navigation
class LcdMenu {
  private:
    LiquidCrystal* _lcd;   // The LCD screen that we'll display the menu on
    MenuItem* _firstItem;  // The first menu item (linked list)
    MenuItem* _lastItem;   // The last menu item
    int _activeId;         // The id of the currently active menu item
    int _longestDisplay;   // The number of characters in the longest menu item
    int _columns;          // The number of columns in the LCD display

    int _leftArrow = 3;
    int _rightArrow = 4;
    int _upArrow = 5;
    int _downArrow = 6;

  public:
    // Create a new menu, using the given LCD screen and using the given number of LCD display columns
    LcdMenu(LiquidCrystal* lcd, int cols) {
      _activeId = 0;
      _lcd = lcd;
      _firstItem = NULL;
      _lastItem = NULL;
      _longestDisplay = 0;
      _columns = cols;
      lcd->createChar(_leftArrow, LeftArr);
      lcd->createChar(_rightArrow, RightArr);
      lcd->createChar(_upArrow, UpArr);
      lcd->createChar(_downArrow, DownArr);
    }

    // Find a menu item by its ID
    MenuItem* findById(int id)
    {
      MenuItem* item = _firstItem;
      while ((item != NULL) && (item->id() != id)) {
        item = item->nextItem();
      }
      return item;
    }

    // Add a new menu item to the list (order matters)
    void addItem(String disp, int id) {
      if (_firstItem == NULL) {
        _firstItem = new MenuItem(disp, id);
        _lastItem = _firstItem;
      }
      else {
        MenuItem* newItem = new MenuItem(disp, id);
        _lastItem->setNextItem(newItem);
        _lastItem = newItem;
      }
      _longestDisplay = max(_longestDisplay, disp.length());
    }

    // Get the currently active item ID
    int getActive() {
      return _activeId;
    }

    // Set the active menu item
    void setActive(int id) {
      _activeId = id;
    }

    // Pass throu utility function
    void setCursor(int col, int row){
      _lcd->setCursor(col,row);
    }
    
    // Go to the next menu item from currently active one
    void setNextActive() {
      // If the last item is active, go to the first.
      if (_lastItem->id() == _activeId) {
        _activeId = _firstItem->id();
      }
      else {
        // Fiund the active one and go to the next
        MenuItem *item = findById(_activeId);
        if (item != NULL) {
          _activeId = item->nextItem()->id();
        }
      }

      // Update the display
      _lcd->setCursor(0, 0);
      updateDisplay();

      // Clear submenu line, in case new menu doesn't print anything.
      _lcd->setCursor(0, 1);
      for (int i = 0; i < _columns; i++) {
        _lcd->print(" ");
      }
    }

    // Update the display of the LCD with the current menu settings
    // This iterates over the menu items, building a menu string by concatenating their display string.
    // It also places the selector arrows around the active one.
    // It then sends teh string to the LCD, keeping the selector arrows centered in the same place.
    void updateDisplay() {
      _lcd->setCursor(0, 0);
      String menuString = "";
      MenuItem* item = _firstItem;
      int offsetToActive = 0;
      int offset = 0;
      // Build the entire menu string
      while (item != NULL) {
        String itemString = "";
        if (item->id() == _activeId) {
          // For the active item remember where it starts in the string and insert selector arrows
          offsetToActive = offset;
          itemString = ">" + item->display() + "<";
        }
        else {
          // For non-active items, pad with a space.
          itemString = " " + item->display() + " ";
        }

        menuString += itemString;
        offset += itemString.length();
        item = item->nextItem();
      }

      // Determine where to place the acive menu item. (empty space around longest item divided by two).
      int margin = (_columns - (_longestDisplay)) / 2;
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

    // Prints a string to the LCD at the current cursor position, using a printf() style call
    void printMenuArg(const char* input, ...) {
      va_list args;
      va_start(args, input);
      int leastCount = 0;
      for (const char* i = input; *i != 0; i++) {
        if (*i != '%') {
          leastCount++;
          if (*i == '>') {
            _lcd->write(_rightArrow);
          }
          else if (*i == '<') {
            _lcd->write(_leftArrow);
          }
          else if (*i == '^') {
            _lcd->write(_upArrow);
          }
          else if (*i == '~') {
            _lcd->write(_downArrow);
          }
          else {
            _lcd->print(*i);
          }
          continue;
        }
        i++;
        leastCount++;
        switch (*i) {
          case '%': _lcd->print('%'); break;
          case 'c':
            {
              byte b = (byte) (va_arg(args, int) && 0x00FF);
              _lcd->write(byte(b));
            }
            break;
          case 's': _lcd->print(va_arg(args, char*)); break;
          case 'd': _lcd->print((int)va_arg(args, int)); break;
          case 'f': _lcd->print((float)va_arg(args, double)); break;
          default: _lcd->print((char)*i); break;
        }
      }
      va_end(args);

      // Since we don't know exaclty how many characters teh var_args printed we know the
      // least count printed and just pad from that (extra spaces are ignored by the LCD).
      while (leastCount < _columns) {
        _lcd->print(" ");
        leastCount++;
      }
    }

    // Print a string to the LCD at the current cursor position, substituting the special arrows and padding with spaces to the end
    void printMenu(String line)
    {
      int spaces = _columns - line.length();
      for (int i = 0; i < line.length(); i++) {
        if (line[i] == '>') {
          _lcd->write(_rightArrow);
        }
        else if (line[i] == '<') {
          _lcd->write(_leftArrow);
        }
        else if (line[i] == '^') {
          _lcd->write(_upArrow);
        }
        else if (line[i] == '~') {
          _lcd->write(_downArrow);
        }
        else {
          _lcd->print(line[i]);
        }
      }

      // Clear the rest of the display
      while (spaces > 0) {
        _lcd->print(" ");
        spaces--;
      }
    }

    // The right arrow bitmap
    byte RightArr[8] = {
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
    byte LeftArr[8] = {
      B00000,
      B00010,
      B00110,
      B01110,
      B00110,
      B00010,
      B00000,
      B00000
    };

    byte UpArr[8] = {
      B00100,
      B01110,
      B11111,
      B00100,
      B00100,
      B00100,
      B00100,
      B00100
    };

    byte DownArr[8] = {
      B000100,
      B000100,
      B000100,
      B000100,
      B000100,
      B011111,
      B001110,
      B000100
    };
};
