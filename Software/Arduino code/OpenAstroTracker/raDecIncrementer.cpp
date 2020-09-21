#include "raDecIncrementer.hpp"

RaDecIncrementer::RaDecIncrementer(String which) : Incrementer(INCREMENT_WRAP, maxDec, minDec)
{
    _isRA = which == "RA";
    if (_isRA)
    {
        setMinVals(minRa);
        setMaxVals(maxRa);
    }
}

void RaDecIncrementer::onChange(String tag, int *numbers, int index, int val)
{
    if ((index == 0) && !_isRA)
    {
        numbers[0] = adjustClamp(numbers[0], val, -90, 90);
    }
    else
    {
        Incrementer::onChange(tag, numbers, index, val);
    }
}

String RaDecIncrementer::getDisplay(String tag, int index, int val, String formatString)
{
    if (index == 3)
    {
        return val == 0 ? "Ta" : "Cu";
    }

    return String(val);
}

void RaDecIncrementer::getNumbers(String tag, int *numbers)
{
    if (_isRA)
    {
        if (numbers[3] == 0)
        {
            DayTime ra = Mount::instance().targetRA();
            numbers[0] = ra.getHours();   // mount get target ra/dec
            numbers[1] = ra.getMinutes(); // mount get target ra/dec
            numbers[2] = ra.getSeconds(); // mount get target ra/dec
        }
        else if (numbers[3] == 1)
        {
            DayTime ra = Mount::instance().currentRA();
            numbers[0] = ra.getHours();   // mount get target ra/dec
            numbers[1] = ra.getMinutes(); // mount get target ra/dec
            numbers[2] = ra.getSeconds(); // mount get target ra/dec
        }
    }
    else
    {
        if (numbers[3] == 0)
        {
            DayTime dec = Mount::instance().targetDEC();
            numbers[0] = dec.getHours();   // mount get target ra/dec
            numbers[1] = dec.getMinutes(); // mount get target ra/dec
            numbers[2] = dec.getSeconds(); // mount get target ra/dec
        }
        else if (numbers[3] == 1)
        {
            DayTime dec = Mount::instance().currentDEC();
            numbers[0] = dec.getHours();   // mount get target ra/dec
            numbers[1] = dec.getMinutes(); // mount get target ra/dec
            numbers[2] = dec.getSeconds(); // mount get target ra/dec
        }
    }
}
