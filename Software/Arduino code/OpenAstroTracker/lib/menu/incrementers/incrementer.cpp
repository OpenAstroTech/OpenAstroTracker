#include "../controls/ChangedEventArgs.hpp"
#include "incrementer.hpp"
#include "../../util/utils.hpp"

Incrementer::Incrementer(int mode = INCREMENT_NO_LIMITS, int *maxVals, int *minVals, eventAction changed)
{
    _mode = mode;
    _menuItem = nullptr;
    _changed = changed;
    _maxVals = maxVals;
    _minVals = minVals;
}

void Incrementer::setMenuItem(MenuItem *menuItem)
{
    _menuItem = menuItem;
}

void Incrementer::onChange(String tag, int *numbers, int index, int val)
{
    int oldVal = numbers[index];
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

    if (_changed != nullptr)
    {
        _changed(new ChangedEventArgs(_menuItem, index, numbers[index], oldVal, numbers));
    }
}

String Incrementer::getDisplay(String tag, int index, int val, String formatString)
{
    if (formatString.length() > 0)
    {
        char achBuffer[20];
        sprintf(achBuffer, formatString.c_str(), val);
        // LOGV4(DEBUG_INFO, "RADEC: Format string is [%s], val is %d, buffer is [%s]", formatString.c_str(), val, achBuffer);
        return String(achBuffer);
    }

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
