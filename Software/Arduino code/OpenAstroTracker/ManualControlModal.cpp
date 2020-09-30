#include "lib/output/LcdDisplay.hpp"
#include "lib/menu/controls/MainMenu.hpp"
#include "ManualControlModal.hpp"
#include "Mount.hpp"

ManualControlModal::ManualControlModal(manualControlEventAction handler, String tag) : MenuItem("", tag)
{
    _handler = handler;
}

void ManualControlModal::onDisplay(bool modal)
{
    char scratchBuffer[20];
    auto mount = Mount::instance();
    sprintf(scratchBuffer, "R%s D%s", mount->RAString(COMPACT_STRING | CURRENT_STRING).c_str(), mount->DECString(COMPACT_STRING | CURRENT_STRING).c_str());
    getMainMenu()->writeToLCD(0, 1, scratchBuffer);
}

bool ManualControlModal::onPreviewKey(int keyState)
{
    if (_keyState != keyState)
    {
        ManualControlEventArgs stateChange(this, ManualControlEventArgs::EventType::ChangeState, _keyState, keyState);
        _handler(&stateChange);
        _keyState = keyState;
    }
    else
    {
        return MenuItem::onPreviewKey(keyState);
    }

    return true;
}

bool ManualControlModal::onKeypressed(int key)
{
    if (key == btnSELECT)
    {
        ManualControlEventArgs mcClose(this, ManualControlEventArgs::EventType::Close);
        _handler(&mcClose);
        return true;
    }
    return false;
}
