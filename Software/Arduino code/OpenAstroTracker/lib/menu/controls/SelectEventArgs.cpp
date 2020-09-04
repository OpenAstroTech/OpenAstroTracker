#include "SelectEventArgs.hpp"

SelectEventArgs::SelectEventArgs(MenuItem *source) : EventArgs(source)
{
}

SelectEventArgs::SelectEventArgs(MenuItem *source, const char *selected) : EventArgs(source)
{
	_selected = selected;
}

const char *SelectEventArgs::getSelected()
{
	return _selected;
}


