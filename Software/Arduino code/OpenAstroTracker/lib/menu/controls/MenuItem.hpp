#ifndef _MENUITEM_HPP_
#define _MENUITEM_HPP_

#include "../list.hpp"

class MainMenu;


class MenuItem
{
protected:
	int _activeSubMenu;
	const char *_displayName;
	const char *_tag;
	List<MenuItem*>* _subMenuList;
	MainMenu *_mainMenu;

public:
	MenuItem(const char *displayName, const char *tag = "");
	void addMenuItem(MenuItem* item);
	const char *getTag() { return _tag; }
	void setTag(const char *tag) { _tag = tag; }
	const char *getDisplayName() { return _displayName; }

	virtual void closeMenuItem(MenuItem* closeMe);
	virtual MainMenu* getMainMenu();
	virtual void setMainMenu(MainMenu* mainMenu);
	virtual void onDisplay(bool modal = false);
	virtual void onSelect();
	virtual bool onPreviewKey(int keyState);
	virtual bool onKeypressed(int key);
};

#endif
