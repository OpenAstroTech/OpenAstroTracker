#include "EventArgs.hpp"

EventArgs::EventArgs(MenuItem *source)
{
	_source = source;
}

MenuItem *EventArgs::getSource() const
{
	return _source;
}
