#include "ChangedEventArgs.hpp"

ChangedEventArgs::ChangedEventArgs(MenuItem *source) : EventArgs(source)
{
}

ChangedEventArgs::ChangedEventArgs(MenuItem *source, int index, int newVal, int oldVal, int *numbers) : EventArgs(source)
{
    _index = index;
    _newVal = newVal;
    _oldVal = oldVal;
    _newNumbers = numbers;
}

int ChangedEventArgs::getIndex()
{
    return _index;
}

int ChangedEventArgs::getNewVal()
{
    return _newVal;
}

int ChangedEventArgs::getOldVal()
{
    return _oldVal;
}

int *ChangedEventArgs::getNumbers()
{
    return _newNumbers;
}
