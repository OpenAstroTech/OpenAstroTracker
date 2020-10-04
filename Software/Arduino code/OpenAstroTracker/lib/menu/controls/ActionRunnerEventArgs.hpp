#pragma once

#include "EventArgs.hpp"

class ActionRunnerEventArgs : public EventArgs
{
public:
    typedef enum
    {
        Starting,
        Running
    } StepState;
    typedef enum
    {
        NoChange,
        Backtrack,
        Proceed,
        Completed
    } StepStateChange;

private:
    int _step;
    StepState _stepState;
    StepStateChange _stepStateChange;
    String _heading;
    String _display;

public:
    ActionRunnerEventArgs(MenuItem *source, int step, StepState state) : EventArgs(source)
    {
        _step = step;
        _stepState = state;
        _stepStateChange = StepStateChange::NoChange;
    }

    int getStep() { return _step; }

    StepState getState() { return _stepState; }
    void setState(StepState state) { _stepState = state; }

    StepStateChange getResult() { return _stepStateChange; }
    void setResult(StepStateChange stateChange) { _stepStateChange = stateChange; }

    String getHeading() { return _heading; }
    void setHeading(String heading) { _heading = heading; }

    String getDisplay() { return _display; }
    void setDisplay(String display) { _display = display; }
};
