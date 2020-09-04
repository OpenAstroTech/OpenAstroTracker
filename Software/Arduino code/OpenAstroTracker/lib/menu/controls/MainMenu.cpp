#include "MainMenu.hpp"

MainMenu::MainMenu()
{
	// _topMenuList = new List<MenuItem*>();
	// _dialogs = new List<MenuItem*>();
	_activeItem = 0;
	_activeDialog = nullptr;
}

void MainMenu::closeDialog()
{
	_activeDialog = nullptr;
}

void MainMenu::activateDialog(const char *tag)
{
	for (int i = 0; i < _dialogs.count(); i++)
	{
		if (_dialogs.getItem(i)->getTag() == tag)
		{
			_activeDialog = _dialogs[i];
			break;
		}
	}

	if (_activeDialog == nullptr)
	{
		//throw new ArgumentException("No dialog named [" + tag + "] was found");
	}
}

void MainMenu::addMenuItem(MenuItem *item)
{
	_topMenuList.add(item);
	item->setMainMenu(this);
}

void MainMenu::addModalDialog(MenuItem *dialog)
{
	dialog->setMainMenu(this);
	_dialogs.add(dialog);
}

bool MainMenu::onPreviewKey(int keyState)
{
	if (_activeDialog != nullptr)
	{
		return _activeDialog->onPreviewKey(keyState);
	}
	return _topMenuList.getItem(_activeItem)->onPreviewKey(keyState);
}

bool MainMenu::processKeys(int key)
{
	if (_activeDialog != nullptr)
	{
		bool waitForRelease = _activeDialog->onKeypressed(key);
		return waitForRelease;
	}

	if (key == btnRIGHT)
	{
		_activeItem = (_activeItem + 1) % _topMenuList.count();
		return true;
	}

	return _topMenuList.getItem(_activeItem)->onKeypressed(key);
}

void MainMenu::updateDisplay()
{
	const char *menu = "";
	if (_activeDialog != nullptr)
	{
		_activeDialog->onDisplay(true);
		return;
	}

	for (int i = 0; i < _topMenuList.count(); i++)
	{
		// TODO: Main menu display on LCD
		//menu += const char *.Format("{0}{1}{2} ", _activeItem == i ? '>' : ' ', _topMenuList[i].getDisplayName(), _activeItem == i ? '<' : ' ');
	}
	//Console.WriteLine(menu);

	_topMenuList.getItem(_activeItem)->onDisplay();
}

void MainMenu::closeMenuItem(MenuItem *closeMe)
{
	if (_activeDialog != nullptr)
	{
		_activeDialog->closeMenuItem(closeMe);
	}
	else
	{
		_topMenuList.getItem(_activeItem)->closeMenuItem(closeMe);
	}
}
