#include "lib/util/utils.hpp"
#include "raDecIncrementer.hpp"

int minDec[] = {-90, 0, 0, 0};
int maxDec[] = {90, 59, 59, 1};
int minRa[] = {0, 0, 0, 0};
int maxRa[] = {23, 59, 59, 1};

RaDecIncrementer::RaDecIncrementer(String which) : Incrementer(INCREMENT_WRAP, maxDec, minDec)
{
    targetOrCurrent = 1;
    _isRA = which == "RA";
    if (_isRA)
    {
        setMinVals(minRa);
        setMaxVals(maxRa);
    }
    else
    {
        setMinVals(minDec);
        setMaxVals(maxDec);
    }
}

void RaDecIncrementer::onChange(String tag, int *numbers, int index, int val)
{
    if ((index == 0) && !_isRA)
    {
        numbers[0] = adjustClamp(numbers[0], val, -180, 0);
    }
    else
    {
        Incrementer::onChange(tag, numbers, index, val);
    }

    targetOrCurrent = numbers[3];

    if (_isRA)
    {
        if (targetOrCurrent)
        {
            Mount::instance()->targetRA().set(numbers[0], numbers[1], numbers[2]);
        }
        else
        {
            // We don't allow editing of current, so switch back to target
            Mount::instance()->targetRA().set(numbers[0], numbers[1], numbers[2]);
            targetOrCurrent = 1;
        }
    }
    else
    {
        if (targetOrCurrent)
        {
            Mount::instance()->targetDEC().set(numbers[0], numbers[1], numbers[2]);
        }
        else
        {
            Mount::instance()->targetDEC().set(numbers[0], numbers[1], numbers[2]);
            targetOrCurrent = 1;
        }
    }
}

String RaDecIncrementer::getDisplay(String tag, int index, int val, String formatString)
{
    if (index == 3)
    {
        return val == 0 ? "Ta" : "Cu";
    }

    if ((index == 0) && !_isRA)
    {
        val = val + 90;
    }

    if (formatString.length() > 0)
    {
        char achBuffer[10];
        sprintf(achBuffer, formatString.c_str(), val);
        LOGV4(DEBUG_INFO, "RADEC: Format string is [%s], val is %d, buffer is [%s]", formatString.c_str(), val, achBuffer);
        return String(achBuffer);
    }
    
    return String(val);
}

void RaDecIncrementer::getNumbers(String tag, int *numbers)
{
    if (_isRA)
    {
        if (targetOrCurrent)
        {
            DayTime ra = Mount::instance()->targetRA();
            numbers[0] = ra.getHours();   // mount get target ra/dec
            numbers[1] = ra.getMinutes(); // mount get target ra/dec
            numbers[2] = ra.getSeconds(); // mount get target ra/dec
            numbers[3] = 0;
        }
        else
        {
            DayTime ra = Mount::instance()->currentRA();
            numbers[0] = ra.getHours();   // mount get target ra/dec
            numbers[1] = ra.getMinutes(); // mount get target ra/dec
            numbers[2] = ra.getSeconds(); // mount get target ra/dec
            numbers[3] = 1;
        }
    }
    else
    {
        if (targetOrCurrent)
        {
            DayTime dec = Mount::instance()->targetDEC();
            numbers[0] = dec.getHours();   // mount get target ra/dec
            numbers[1] = dec.getMinutes(); // mount get target ra/dec
            numbers[2] = dec.getSeconds(); // mount get target ra/dec
            numbers[3] = 0;
        }
        else
        {
            DayTime dec = Mount::instance()->currentDEC();
            numbers[0] = dec.getHours();   // mount get target ra/dec
            numbers[1] = dec.getMinutes(); // mount get target ra/dec
            numbers[2] = dec.getSeconds(); // mount get target ra/dec
            numbers[3] = 1;
        }
    }
}
