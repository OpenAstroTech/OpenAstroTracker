#ifndef _SELECTEVENTARGs_HPP_
#define _SELECTEVENTARGs_HPP_

#include "EventArgs.hpp"

class MenuItem;

class SelectEventArgs : public EventArgs
{
private:
	const char *_selected;

public:
	SelectEventArgs(MenuItem *source);
	SelectEventArgs(MenuItem *source, const char *selected);
	const char *getSelected();
};

#endif
