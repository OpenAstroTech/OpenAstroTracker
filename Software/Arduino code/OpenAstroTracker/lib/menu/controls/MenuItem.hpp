#pragma once

#include <Arduino.h>
#include "../list.hpp"

class MainMenu;


class MenuItem
{
protected:
	int _activeSubMenu;
	String _displayName;
	String _tag;
	List<MenuItem*>* _subMenuList;
	MainMenu *_mainMenu;

public:
	MenuItem(String displayName, String tag);
	void addMenuItem(MenuItem* item);
	String getTag() { return _tag; }
	void setTag(String tag) { _tag = tag; }
	String getDisplayName() { return _displayName; }

	virtual void closeMenuItem(MenuItem* closeMe);
	virtual MainMenu* getMainMenu();
	virtual void setMainMenu(MainMenu* mainMenu);
	virtual void onDisplay(bool modal = false);
	virtual void onSelect();
	virtual bool onPreviewKey(int keyState);
	virtual bool onKeypressed(int key);
};

