#include "../../input/LcdButtons.hpp"
#include "../incrementers/incrementer.hpp"
#include "SelectEventArgs.hpp"
#include "MainMenu.hpp"
#include "NumberInput.hpp"

NumberInput::NumberInput(String tag, int numNumbers, String mask, eventAction selectCallback, Incrementer *incr, BehaviorFlags flags) : MenuItem("", tag.c_str())
{
    _activeNumber = 0;
    _mask = mask;
    _numbers = new int[numNumbers];
    _numNumbers = numNumbers;
    _selectCallback = selectCallback;
    _flags = flags;
    _incr = (incr == nullptr) ? new Incrementer() : incr;
    _useIncrForDisplay = true;
    _incr->setMenuItem(this);
}

NumberInput::NumberInput(String tag, int *numbers, int numNumbers, String mask, eventAction selectCallback, Incrementer *incr, BehaviorFlags flags) : MenuItem("", tag.c_str())
{
    _activeNumber = 0;
    _mask = mask;
    _numbers = numbers;
    _numNumbers = numNumbers;
    _selectCallback = selectCallback;
    _flags = flags;
    _incr = (incr == nullptr) ? new Incrementer() : incr;
    _incr->setMenuItem(this);
    _useIncrForDisplay = false;
}

void NumberInput::onDisplay(bool modal = false)
{
    int cursor = _activeNumber;
    int numberCursor = 0;
    String line = "";
    if (_useIncrForDisplay)
    {
        _incr->getNumbers(_tag, _numbers);
    }
    for (int i = 0; i < _mask.length(); i++)
    {
        if (_mask[i] == '^')
        {
            line += cursor == 0 ? '>' : ' ';
            cursor--;
        }
        else if (_mask[i] == '@')
        {
            line += _incr->getDisplay(_tag, numberCursor, _numbers[numberCursor]);
            numberCursor++;
        }
        else if (_mask[i] == '%')
        {
            int start = i;
            while (!isalpha(_mask[i]))
            {
                i++;
            }
            
            line += _incr->getDisplay(_tag, numberCursor, _numbers[numberCursor], _mask.substring(start, i + 1));
            numberCursor++;
        }
        else
        {
            line += _mask[i];
        }
    }

    _mainMenu->writeToLCD(0, 1, line);
}

void NumberInput::onSelect()
{
    if (_selectCallback != nullptr)
    {
        _selectCallback(new SelectEventArgs(this));
    }
    else
    {
        _incr->onSelect(new SelectEventArgs(this));
    }
}

int NumberInput::getValue(int index)
{
    return _numbers[index];
}

bool NumberInput::onPreviewKey(int keyState)
{
    if (keyState == btnDOWN)
    {
        if (millis() > _nextKeyOk)
        {
            _incr->onChange(_tag, _numbers, _activeNumber, -1);
            _waitFor = (_flags == NumberInput::BehaviorFlags::AcceleratingRepetition) ? (unsigned long)max(10, 0.95 * _waitFor) : 200;
            _nextKeyOk = millis() + _waitFor;
        }
    }
    else if (keyState == btnUP)
    {
        if (millis() > _nextKeyOk)
        {
            _incr->onChange(_tag, _numbers, _activeNumber, 1);
            _waitFor = (_flags == NumberInput::BehaviorFlags::AcceleratingRepetition) ? (unsigned long)max(10, 0.95 * _waitFor) : 200;
            _nextKeyOk = millis() + _waitFor;
        }
    }
    else
    {
        _nextKeyOk = 0;
        _waitFor = 200;
        return false;
    }

    return true;
}

bool NumberInput::onKeypressed(int key)
{
    if (!MenuItem::onKeypressed(key))
    {
        if (key == btnLEFT)
        {
            _activeNumber = (_activeNumber + 1) % _numNumbers;
            return true;
        }
    }
    return false;
}
