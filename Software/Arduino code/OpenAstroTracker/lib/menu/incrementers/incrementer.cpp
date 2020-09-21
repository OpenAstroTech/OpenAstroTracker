#include "incrementer.hpp"

Incrementer::Incrementer(int mode = INCREMENT_NO_LIMITS, int *maxVals = nullptr, int *minVals = nullptr)
{
    _mode = mode;
    _maxVals = maxVals;
    _minVals = minVals;
}

void Incrementer::onChange(String tag, int *numbers, int index, int val)
{
    if (_mode == INCREMENT_CLAMP)
    {
        numbers[index] = adjustClamp(numbers[index], val, (_minVals != nullptr) ? _minVals[index] : 0, _maxVals[index]);
    }
    else if (_mode == INCREMENT_WRAP)
    {
        numbers[index] = adjustWrap(numbers[index], val, (_minVals != nullptr) ? _minVals[index] : 0, _maxVals[index]);
    }
    else if (_mode == INCREMENT_NO_LIMITS)
    {
        numbers[index] += val;
    }
}

String Incrementer::getDisplay(String tag, int index, int val, String formatString = "")
{
    return String(val);
}

void Incrementer::setMinVals(int *minVals)
{
    _minVals = minVals;
}
void Incrementer::setMaxVals(int *maxVals)
{
    _maxVals = maxVals;
}
