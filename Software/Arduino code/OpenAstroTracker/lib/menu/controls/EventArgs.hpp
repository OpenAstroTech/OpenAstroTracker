#ifndef _EVENTARGS_HPP_
#define _EVENTARGS_HPP_

#include "MenuItem.hpp"

class EventArgs
{
private:
	MenuItem* _source;
public:
	EventArgs(MenuItem* source);
	MenuItem* getSource() const;
};

typedef void (*eventAction)(EventArgs*);

#endif
