#include "LcdButtons.hpp"

LcdButtons::LcdButtons(byte pin)
{
    _analogPin = pin;
    _lastKeyChange = 0;

    _newKey = -1;
    _lastNewKey = -2;

    _currentKey = -1;
    _lastKey = -2;
}

byte LcdButtons::currentKey()
{
    checkKey();
    return _newKey;
}

byte LcdButtons::currentState()
{
    checkKey();
    return _currentKey;
}

int LcdButtons::currentAnalogState()
{
    checkKey();
    return _analogKeyValue;
}

bool LcdButtons::keyChanged(byte *pNewKey)
{
    checkKey();
    if (_newKey != _lastNewKey)
    {
        *pNewKey = _newKey;
        _lastNewKey = _newKey;
        return true;
    }
    return false;
}

void LcdButtons::checkKey()
{
    _analogKeyValue = analogRead(_analogPin);
    if (_analogKeyValue > 1000)
        _currentKey = btnNONE;
    else if (_analogKeyValue < 50)
        _currentKey = btnRIGHT;
    else if (_analogKeyValue < 240)
        _currentKey = btnUP;
    else if (_analogKeyValue < 400)
        _currentKey = btnDOWN;
    else if (_analogKeyValue < 600)
        _currentKey = btnLEFT;
    else if (_analogKeyValue < 920)
        _currentKey = btnSELECT;

    if (_currentKey != _lastKey)
    {
        _lastKey = _currentKey;
        _lastKeyChange = millis();
    }
    else
    {
        // If the keys haven't changed in 5ms, commit the change to the new keys.
        if (millis() - _lastKeyChange > 5)
        {
            _newKey = _currentKey;
        }
    }
}
