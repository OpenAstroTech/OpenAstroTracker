#ifndef _LCDMENU_HPP_
#define _LCDMENU_HPP_

#include <Arduino.h>
#ifndef HEADLESS_CLIENT
#include <LiquidCrystal.h>
#endif
#include "Globals.h"

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
  public:
    // Create a new menu, using the given number of LCD display columns and rows
    LcdMenu(byte cols, byte rows, int maxItems);

    // Find a menu item by its ID
    MenuItem* findById(byte id);

    // Add a new menu item to the list (order matters)
    void addItem(const char *disp, byte id);

    // Get the currently active item ID
    byte getActive();

    // Set the active menu item
    void setActive(byte id);

    // Pass thru utility function
    void setCursor(byte col, byte row);

    // Pass thru utility function
    void clear();

    // Go to the next menu item from currently active one
    void setNextActive();

    // Update the display of the LCD with the current menu settings
    // This iterates over the menu items, building a menu string by concatenating their display string.
    // It also places the selector arrows around the active one.
    // It then sends the string to the LCD, keeping the selector arrows centered in the same place.
    void updateDisplay();

    // Print a string to the LCD at the current cursor position, substituting the special arrows and padding with spaces to the end
    void printMenu(String line);

  private:
    void printChar(char ch);

  private:
#ifndef HEADLESS_CLIENT
    LiquidCrystal _lcd;   // The LCD screen that we'll display the menu on
    MenuItem** _menuItems;  // The first menu item (linked list)
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

    // The special character bitmaps
    static byte RightArrowBitmap[8];
    static byte LeftArrowBitmap[8];
    static byte UpArrowBitmap[8];
    static byte DownArrowBitmap[8];
    static byte DegreesBitmap[8];
    static byte MinutesBitmap[8];
#endif
};

#endif
