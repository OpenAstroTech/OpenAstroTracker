#ifndef _LCDDISPLAY_HPP_
#define _LCDDISPLAY_HPP_

#include <Arduino.h>
#include <LiquidCrystal.h>

// Class that drives the LCD screen 
class LcdDisplay {
public:
  // Create a new display, using the given number of LCD display columns and rows
  LcdDisplay(byte cols, byte rows);

  // Instance of the display
  static LcdDisplay* instance();

  // The LCD screen that we're using
  LiquidCrystal* getLCD() {return &_lcd; }   

  // Pass thru utility function
  void clear();

  // Pass thru utility function
  void setCursor(byte col, byte row);

  // Print a string to the LCD at the current cursor position, substituting the special arrows and padding with spaces to the end
  void printLine(String line);

  // Print a character at a specific position
  void printAt(int col, int row, char ch);

private:
  // Print a single character at the current cursor location and advance cursor by one. Substitutes special chars.
  void printChar(char ch);

private:
#if HEADLESS_CLIENT == 0
  LiquidCrystal _lcd;     // The LCD screen that we'll display on
  byte _columns;          // The number of columns in the LCD display
  byte _activeRow;        // The row that the LCD cursor is on
  byte _activeCol;        // The column that the LCD cursor is on
  String _lastDisplay[2]; // The last string that was displayed on each row
  int _brightness;

  //byte _degrees = 1;
  byte _degrees = 1;
  byte _minutes = 2;
  byte _leftArrow = 3;
  byte _rightArrow = 4;
  byte _upArrow = 5;
  byte _downArrow = 6;
  byte _tracking = 7;
  byte _noTracking = 0;

  // The special character bitmaps
  static byte RightArrowBitmap[8];
  static byte LeftArrowBitmap[8];
  static byte UpArrowBitmap[8];
  static byte DownArrowBitmap[8];
  static byte DegreesBitmap[8];
  static byte MinutesBitmap[8];
  static byte TrackingBitmap[8];
  static byte NoTrackingBitmap[8];

  static LcdDisplay* _instance;
#endif
};

#endif
