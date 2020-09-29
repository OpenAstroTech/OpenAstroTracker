#include <Arduino.h>

#include "SelectEventArgs.hpp"

SelectEventArgs::SelectEventArgs(MenuItem *source) : EventArgs(source)
{
}

SelectEventArgs::SelectEventArgs(MenuItem *source, String selected) : EventArgs(source)
{
	_selected = selected;
}

String SelectEventArgs::getSelected()
{
	return _selected;
}


