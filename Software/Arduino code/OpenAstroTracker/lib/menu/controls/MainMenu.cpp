
#include "MenuItem.hpp"
#include "MainMenu.hpp"
#include "MainMenu.hpp"
#include "../../util/debug.hpp"
#include "../../input/LcdButtons.hpp"

MainMenu::MainMenu(LcdDisplay *lcdDisplay) : _lcdDisplay(lcdDisplay)
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
			_activeDialog = _dialogs.getItem(i);
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
		int last = _activeItem;
		_activeItem = (_activeItem + 1) % _topMenuList.count();
		//LOGV3(255, "MMN: Moving to next menu from %s to %s", _topMenuList.getItem(last)->getTag(), _topMenuList.getItem(_activeItem)->getTag());
		// LOGV3(255, "MMN: Moving to next menu from %d to %d", last, _activeItem);
		return true;
	}

	return _topMenuList.getItem(_activeItem)->onKeypressed(key);
}

void MainMenu::updateDisplay()
{
	String menu = "    ";
	if (_activeDialog != nullptr)
	{
		_activeDialog->onDisplay(true);
		return;
	}

	for (int i = 0; i < _topMenuList.count(); i++)
	{
		menu += _activeItem == i ? '>' : ' ';
		menu += _topMenuList.getItem(i)->getDisplayName();
		menu += _activeItem == i ? '<' : ' ';
	}

	int activeItemPos = menu.indexOf('>');
	menu = menu.substring(activeItemPos - 4);
	for (int i = menu.length(); i < 14; i++)
	{
		menu += ' ';
	}
	menu[0] = '|';
	menu[14] = '|';
	_lcdDisplay->setCursor(0, 0);
	// LOGV2(DEBUG_INFO, "MMUD: Menu  is [%s]", menu.substring(activeItemPos - 4).c_str());
	_lcdDisplay->printLine(menu.substring(0,14));

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

void MainMenu::writeToLCD(int col, int row, String text)
{
	_lcdDisplay->setCursor(col, row);
	_lcdDisplay->printLine(text);
}
