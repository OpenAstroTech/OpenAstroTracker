#pragma once
#include "EventArgs.hpp"
#include "MenuItem.hpp"

class SelectEventArgs : EventArgs
{
private:
	const char *_selected;

public:
	SelectEventArgs(MenuItem *source);
	SelectEventArgs(MenuItem *source, const char *selected);
	const char *getSelected();
};
