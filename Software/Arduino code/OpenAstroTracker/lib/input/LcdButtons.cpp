#include "../util/debug.hpp"
#include "LcdButtons.hpp"

LcdButtons *LcdButtons::_instance = nullptr;
LcdButtons *LcdButtons::instance() { return _instance; };

// const char* keyNames[] = {
//       "RIGHT ",
//       "UP    ",
//       "DOWN  ",
//       "LEFT  ",
//       "SELECT",
//       "NONE  "
// };

// const char *keyNames(int key)
// {
//     if (key == 0)
//         return "RIGHT ";
//     if (key == 1)
//         return "UP    ";
//     if (key == 2)
//         return "DOWN  ";
//     if (key == 3)
//         return "LEFT  ";
//     if (key == 4)
//         return "SELECT";
//     if (key == 5)
//         return "NONE  ";
//     return "WTF  ";
// };

LcdButtons::LcdButtons(byte pin)
{
    _instance = this;
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
        // LOGV2(255, "BTN: KeyChanged returning true. New key %s", keyNames(_newKey));
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
        //LOGV3(255, "BTN: Key state changed from %s to %s", keyNames(_lastKey), keyNames(_currentKey));
        _lastKey = _currentKey;
        _lastKeyChange = millis();
    }
    else
    {
        // If the keys haven't changed in 5ms, commit the change to the new keys.
        if (millis() - _lastKeyChange > 5)
        {
            // if (_newKey != _currentKey)
            // {
            //     LOGV2(255, "BTN: Committing %s as new key, 5ms passed", keyNames(_currentKey));
            // }
            _newKey = _currentKey;
        }
    }
}
