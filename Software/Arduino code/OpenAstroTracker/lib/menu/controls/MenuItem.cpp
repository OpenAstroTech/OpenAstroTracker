#include <Arduino.h>
#include "../../input/LcdButtons.hpp"
#include "MenuItem.hpp"
#include "MainMenu.hpp"

MenuItem::MenuItem(String displayName, String tag)
{
	_tag = tag;
	_displayName = displayName;
	_subMenuList = new List<MenuItem *>();
	_activeSubMenu = 0;
}

void MenuItem::addMenuItem(MenuItem *item)
{
	_subMenuList->add(item);
}

void MenuItem::closeMenuItem(MenuItem *closeMe)
{
	if (_subMenuList->count() > 0)
	{
		_subMenuList->getItem(_activeSubMenu)->closeMenuItem(closeMe);
	}
}

MainMenu *MenuItem::getMainMenu()
{
	return _mainMenu;
}

void MenuItem::setMainMenu(MainMenu *mainMenu)
{
	_mainMenu = mainMenu;
	for (int i = 0; i < _subMenuList->count(); i++)
	{
		_subMenuList->getItem(i)->setMainMenu(mainMenu);
	}
}

void MenuItem::onDisplay(bool modal)
{
	if (modal)
	{
		getMainMenu()->writeToLCD(0,0, _displayName);
	}
	if (_subMenuList->count() > 0)
	{
		_subMenuList->getItem(_activeSubMenu)->onDisplay(modal);
	}
}

void MenuItem::onSelect() {}

bool MenuItem::onPreviewKey(int keyState)
{
	if (_subMenuList->count() > 0)
	{
		return _subMenuList->getItem(_activeSubMenu)->onPreviewKey(keyState);
	}
	return false;
}

// True if should wait for release
bool MenuItem::onKeypressed(int key)
{
	if ((key == btnRIGHT) && (_subMenuList->count() > 0))
	{
		_activeSubMenu = (_activeSubMenu + 1) % _subMenuList->count();
		return true;
	}

	if (_subMenuList->count() > 0)
	{
		if (key == btnSELECT)
		{
			_subMenuList->getItem(_activeSubMenu)->onSelect();
			return true;
		}
		return _subMenuList->getItem(_activeSubMenu)->onKeypressed(key);
	}
	return false;
}
