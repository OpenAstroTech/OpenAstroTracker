#include <Arduino.h>

#include "ManualControlEventArgs.hpp"

ManualControlEventArgs::ManualControlEventArgs(MenuItem* source, EventType evt, int oldState = 0, int newState = 0) : EventArgs(source)
{
    _evt = evt;
    _oldState = oldState;
    _newState = newState;
}

ManualControlEventArgs::EventType ManualControlEventArgs::getEvent()
{
    return _evt;
}

int ManualControlEventArgs::getOldState()
{
    return _oldState;
}

int ManualControlEventArgs::getNewState()
{
    return _newState;
}
