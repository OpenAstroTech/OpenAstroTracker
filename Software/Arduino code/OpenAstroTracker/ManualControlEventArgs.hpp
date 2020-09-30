#pragma once

#include "lib/menu/controls/EventArgs.hpp"

class MenuItem;

class ManualControlEventArgs : public EventArgs
{
public:
    typedef enum
    {
        Close,
        ChangeState
    } EventType;
private:
    EventType _evt;
    int _oldState;
    int _newState;

public:
    ManualControlEventArgs(MenuItem *source, EventType evt, int oldState = 0, int newState = 0);

    EventType getEvent();
    int getOldState();
    int getNewState();
};

typedef void (*manualControlEventAction)(ManualControlEventArgs*);