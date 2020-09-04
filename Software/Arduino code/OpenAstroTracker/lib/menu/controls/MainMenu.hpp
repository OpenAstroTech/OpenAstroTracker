#ifndef _MAINMENU_HPP_
#define _MAINMENU_HPP_

#include "../list.hpp"

class MenuItem;

class MainMenu
{
private:
	MenuItem *_activeDialog;
	int _activeItem;
	List<MenuItem*> _topMenuList;
	List<MenuItem*> _dialogs;

public:
	MainMenu();

	void addMenuItem(MenuItem* item);

	void closeDialog();
	void activateDialog(const char *tag);
	void addModalDialog(MenuItem* dialog);

	bool onPreviewKey(int keyState);
	bool processKeys(int key);

	void updateDisplay();

	virtual void closeMenuItem(MenuItem* closeMe);
};

#endif
