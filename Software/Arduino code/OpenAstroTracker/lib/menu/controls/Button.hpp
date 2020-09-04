#include "EventArgs.hpp"

class Button : MenuItem
{
private:
	eventAction _chosenFunc;

	MenuItem *_subMenu;
	bool _subMenuActivated;
public:
	Button(const char *choice, MenuItem *subMenu);
	Button(const char *choice, eventAction chosenFunc);

	virtual void closeMenuItem(MenuItem *closeMe) override;
	virtual void setMainMenu(MainMenu* mainMenu) override;

	virtual void onDisplay(bool modal = false) override;
	virtual void onSelect() override;

	virtual bool onPreviewKey(int keyState) override;
	virtual bool onKeypressed(int key) override;
};