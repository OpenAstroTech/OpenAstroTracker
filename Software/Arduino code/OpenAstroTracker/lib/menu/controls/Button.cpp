#include "Button.hpp"

Button::Button(const char *choice, MenuItem *subMenu) : MenuItem(choice)
{
	_chosenFunc = nullptr;
	_subMenu = subMenu;
	_subMenuActivated = false;
}

Button::Button(const char *choice, eventAction chosenFunc) : MenuItem(choice)
{
	_chosenFunc = chosenFunc;
	_subMenu = nullptr;
	_subMenuActivated = false;
}

void Button::closeMenuItem(MenuItem *closeMe) override
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

bool Button::onKeypressed(int key) override
{
	if (_subMenuActivated)
	{
		return _subMenu->onKeypressed(key);
	}
	return MenuItem::onKeypressed(key);
}

bool Button::onPreviewKey(int keyState) override
{
	if (_subMenuActivated)
	{
		return _subMenu->onPreviewKey(keyState);
	}
	return MenuItem::onPreviewKey(keyState);
}

void Button::setMainMenu(MainMenu *mainMenu) override
{
	MenuItem::setMainMenu(mainMenu);
	if (_subMenu != nullptr)
	{
		_subMenu->setMainMenu(mainMenu);
	}
}

void Button::onDisplay(bool modal = false) override
{
	if ((_subMenu != nullptr) && _subMenuActivated)
	{
		_subMenu->onDisplay(modal);
	}
	else
	{
		// const char * line = ">" + _displayName;
		// TODO: LCD display
		//Console.WriteLine(line);
	}
}

void Button::onSelect() override
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
