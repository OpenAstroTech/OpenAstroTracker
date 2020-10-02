#include "../../input/LcdButtons.hpp"
#include "MainMenu.hpp"
#include "MultiTextInfo.hpp"

MultiTextInfo::MultiTextInfo(int numSubItems, multiDisplayFunc displayFunc) : MenuItem("MT","MT")
{
    _numSubItems = numSubItems;
    _currentSubItem = 0;
    _displayFunc = displayFunc;
}

bool MultiTextInfo::onKeypressed(int key)
{
    if (key == btnLEFT)
    {
        _currentSubItem = (_currentSubItem + 1) % _numSubItems;
        return true;
    }
    return MenuItem::onKeypressed(key);
}

void MultiTextInfo::onDisplay(bool modal)
{
    getMainMenu()->writeToLCD(0, 1, _displayFunc(_currentSubItem));
}
