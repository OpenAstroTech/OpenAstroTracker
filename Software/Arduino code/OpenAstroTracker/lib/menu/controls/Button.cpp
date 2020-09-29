#include <Arduino.h>

#include "SelectEventArgs.hpp"
#include "MainMenu.hpp"
#include "Button.hpp"

Button::Button(String choice, MenuItem *subMenu) : MenuItem(choice, "BTN")
{
	_chosenFunc = nullptr;
	_subMenu = subMenu;
	_subMenuActivated = false;
}

Button::Button(String choice, eventAction chosenFunc) : MenuItem(choice, "BTN")
{
	_chosenFunc = chosenFunc;
	_subMenu = nullptr;
	_subMenuActivated = false;
}

void Button::closeMenuItem(MenuItem *closeMe)
{
	if (_subMenuActivated)
	{
		if (_subMenu == closeMe)
		{
			_subMenuActivated = false;
		}
		else
		{
			_subMenu->closeMenuItem(closeMe);
		}
	}
}

bool Button::onKeypressed(int key)
{
	if (_subMenuActivated)
	{
		return _subMenu->onKeypressed(key);
	}
	return MenuItem::onKeypressed(key);
}

bool Button::onPreviewKey(int keyState)
{
	if (_subMenuActivated)
	{
		return _subMenu->onPreviewKey(keyState);
	}
	return MenuItem::onPreviewKey(keyState);
}

void Button::setMainMenu(MainMenu *mainMenu)
{
	MenuItem::setMainMenu(mainMenu);
	if (_subMenu != nullptr)
	{
		_subMenu->setMainMenu(mainMenu);
	}
}

void Button::onDisplay(bool modal)
{
	if ((_subMenu != nullptr) && _subMenuActivated)
	{
		_subMenu->onDisplay(modal);
	}
	else
	{
		_mainMenu->writeToLCD(0, 1, String(">") + _displayName);
	}
}

void Button::onSelect()
{
	if (_subMenu != nullptr)
	{
		if (_subMenuActivated)
		{
			_subMenu->onSelect();
			_subMenuActivated = false;
		}
		else
		{
			_subMenuActivated = true;
		}
	}
	else
	{
		_chosenFunc(new SelectEventArgs(this, _displayName));
	}
}
