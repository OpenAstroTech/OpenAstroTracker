#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_

#include "EventArgs.hpp"
#include "MenuItem.hpp"

class Button : public MenuItem
{
private:
	eventAction _chosenFunc;

	MenuItem *_subMenu;
	bool _subMenuActivated;

public:
	Button(String choice, MenuItem *subMenu);
	Button(String choice, eventAction chosenFunc);

	virtual void closeMenuItem(MenuItem *closeMe);
	virtual void setMainMenu(MainMenu *mainMenu);

	virtual void onDisplay(bool modal = false);
	virtual void onSelect();

	virtual bool onPreviewKey(int keyState);
	virtual bool onKeypressed(int key);
};

#endif