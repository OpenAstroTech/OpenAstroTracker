class MenuItem {
    String _display;
    int _id;
    MenuItem* _nextItem;
  public:
    MenuItem(String display, int id)
    {
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

class LcdMenu {
  private:
    LiquidCrystal* _lcd;
    MenuItem* _firstItem;
    MenuItem* _lastItem;
    int _activeId;
    int _longestDisplay;
    int _columns;

    int _leftArrow = 3;
    int _rightArrow = 4;

  public:
    LcdMenu(LiquidCrystal* lcd, int cols) {
      _activeId = 0;
      _lcd = lcd;
      _firstItem = NULL;
      _lastItem = NULL;
      _longestDisplay = 0;
      _columns = cols;
      lcd->createChar(_leftArrow, LeftArr);
      lcd->createChar(_rightArrow, RightArr);
    }

    MenuItem* findById(int id)
    {
      MenuItem* item = _firstItem;
      while ((item != NULL) && (item->id() != id)) {
        item = item->nextItem();
      }
      return item;
    }

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

      updateDisplay();
    }

    void setActive(int id) {
      _activeId = id;
    }

    void setNextActive() {
      if (_lastItem->id() == _activeId) {
        _activeId = _firstItem->id();
      }
      else {
        MenuItem *item = findById(_activeId);
        if (item != NULL) {
          _activeId = item->nextItem()->id();
        }
      }
    }

    int getActive() {
      return _activeId;
    }

    void updateDisplay() {
      _lcd->setCursor(0, 0);
      String menuString = "";
      MenuItem* item = _firstItem;
      int offsetToActive = 0;
      int offset = 0;
      while (item != NULL) {
        String itemString = "";
        if (item->id() == _activeId) {
          offsetToActive = offset;
          itemString = ">" + item->display() + "<";
        }
        else {
          itemString = " " + item->display() + " ";
        }
        menuString += itemString;
        offset += itemString.length();
        item = item->nextItem();
      }
      int margin = (_columns - (_longestDisplay)) / 2;
      int offsetIntoString = offsetToActive - margin;

      while (offsetIntoString < 0) {
        menuString = " " + menuString;
        offsetIntoString++;
      }

      String displayString = menuString.substring(offsetIntoString, offsetIntoString + _columns);
      while (displayString.length() < _columns)
      {
        displayString += " ";
      }
      printMenu(displayString);
    }

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
              _lcd->write((byte)b);
            }
            break;
          case 's': _lcd->print(va_arg(args, char*)); break;
          case 'd': _lcd->print((int)va_arg(args, int)); break;
          case 'f': _lcd->print((float)va_arg(args, float)); break;
          default: _lcd->print((char)*i); break;
        }
      }
      va_end(args);
      while (leastCount < _columns) {
        _lcd->print(" ");
        leastCount++;
      }
    }

    // Print the given string to the lcd, substituting the special arrows and padding with spaces to the end
    void printMenu(String line)
    {
      int spaces = _columns - line.length();
      for (int i = 0; i < line.length(); i++) {
        if (line[i] == ' > ') {
          _lcd->write(_rightArrow);
        }
        else if (line[i] == ' < ') {
          _lcd->write(_leftArrow);
        }
        else {
          _lcd->print(line[i]);
        }
      }

      // Clear the rest of the display
      while (spaces > 0) {
        _lcd->print(" ");
      }
    }

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

};
