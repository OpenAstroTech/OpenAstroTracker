#pragma once

#include "lib/menu/controls/MenuItem.hpp"
#include "lib/input/LcdButtons.hpp"
#include "ManualControlEventArgs.hpp"

class ManualControlModal : public MenuItem
{
private:
    manualControlEventAction _handler;
    int _keyState = btnNONE;

public:
    ManualControlModal(manualControlEventAction handler, String tag);

    void onDisplay(bool modal = false) override;
    bool onPreviewKey(int keyState) override;
    bool onKeypressed(int key) override;
};
