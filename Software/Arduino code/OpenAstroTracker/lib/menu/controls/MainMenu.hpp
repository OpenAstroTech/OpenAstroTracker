#ifndef _MAINMENU_HPP_
#define _MAINMENU_HPP_

#include "../list.hpp"
#include "LiquidCrystal.h"

class MenuItem;

class MainMenu
{
private:
	MenuItem *_activeDialog;
	int _activeItem;
	List<MenuItem *> _topMenuList;
	List<MenuItem *> _dialogs;
	LiquidCrystal *_lcdMenu;

public:
	MainMenu(LiquidCrystal *lcdMenu);

	void writeToLCD(int col, int row, String text);
	
	void addMenuItem(MenuItem *item);

	void closeDialog();
	void activateDialog(const char *tag);
	void addModalDialog(MenuItem *dialog);

	bool onPreviewKey(int keyState);
	bool processKeys(int key);

	void updateDisplay();

	virtual void closeMenuItem(MenuItem *closeMe);
};

#endif
