//#include "Utility.hpp"
//#include "EPROMStore.hpp"
#include "LcdDisplay.hpp"

LcdDisplay* LcdDisplay::_instance = nullptr;
LcdDisplay* LcdDisplay::instance() { return _instance; }

// Class that drives the LCD screen with a menu
// You add a string and an id item and this class handles the display and navigation
// Create a new menu, using the given number of LCD display columns and rows
LcdDisplay::LcdDisplay(byte cols, byte rows) : _lcd(8, 9, 4, 5, 6, 7) {
  _instance = this;

  _lcd.begin(cols, rows);
  _columns = cols;
  _activeRow = -1;
  _activeCol = -1;
  _lastDisplay[0] = "";
  _lastDisplay[1] = "";

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

// Pass thru utility function
void LcdDisplay::setCursor(byte col, byte row) {
  _activeRow = row;
  _activeCol = col;
}

// Pass thru utility function
void LcdDisplay::clear() {
  _lcd.clear();
}

// Print the given character to the LCD, converting some special ones to our bitmaps
void LcdDisplay::printChar(char ch) {
  if (ch == '>') {
    _lcd.write(_rightArrow);
  }
  else if (ch == '<') {
    _lcd.write(_leftArrow);
  }
  else if (ch == '^') {
    _lcd.write(_upArrow);
  }
  else if (ch == '~') {
    _lcd.write(_downArrow);
  }
  else if (ch == '*') {
    _lcd.write(_degrees);
  }
  else if (ch == '\'') {
    _lcd.write(_minutes);
  }
  else if (ch == '&') {
    _lcd.write(_tracking);
  }
  else if (ch == '`') {
    _lcd.write(_noTracking);
  }
  else {
    _lcd.print(ch);
  }
}

// Print a character at a specific position
void LcdDisplay::printAt(int col, int row, char ch) {
  _lcd.setCursor(col, row);
  printChar(ch);
}

// Print a string to the LCD at the current cursor position, substituting the special arrows and padding with spaces to the end
void LcdDisplay::printLine(String line) {
  if ((_lastDisplay[_activeRow] != line) || (_activeCol != 0)) {

    _lastDisplay[_activeRow] = line;

    _lcd.setCursor(_activeCol, _activeRow);
    int spaces = _columns - line.length();
    for (unsigned int i = 0; i < line.length(); i++) {
      printChar(line[i]);
    }

    // Clear the rest of the display
    while (spaces > 0) {
      _lcd.print(" ");
      spaces--;
    }
  }
}

// The right arrow bitmap
byte LcdDisplay::RightArrowBitmap[8] = {
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
byte LcdDisplay::LeftArrowBitmap[8] = {
  B00000,
  B00010,
  B00110,
  B01110,
  B00110,
  B00010,
  B00000,
  B00000
};

byte LcdDisplay::UpArrowBitmap[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte LcdDisplay::DownArrowBitmap[8] = {
  B000100,
  B000100,
  B000100,
  B000100,
  B000100,
  B011111,
  B001110,
  B000100
};

byte LcdDisplay::DegreesBitmap[8] = {
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
  B00000
};

byte LcdDisplay::MinutesBitmap[8] = {
  B01000,
  B01000,
  B01000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};


byte LcdDisplay::TrackingBitmap[8] = {
  B10111,
  B00010,
  B10010,
  B00010,
  B10111,
  B00101,
  B10110,
  B00101
};


byte LcdDisplay::NoTrackingBitmap[8] = {
  B10000,
  B00000,
  B10000,
  B00010,
  B10000,
  B00000,
  B10000,
  B00000
};

// byte LcdDisplay::TrackingBitmap[8] = {
//   B10000,
//   B00000,
//   B10111,
//   B00010,
//   B10010,
//   B00010,
//   B10000,
//   B00000
// };

// byte LcdDisplay::TrackingBitmap[8] = {
//   B11100,
//   B01000,
//   B01000,
//   B01000,
//   B00111,
//   B00101,
//   B00110,
//   B00101
// };