#pragma once

#include <Arduino.h>
#include "../list.hpp"
#include "EventArgs.hpp"
#include "MenuItem.hpp"

class OptionChooser : public MenuItem
{
private:
    int _activeOption;
    String _choices[3];
    int _numChoices;
    eventAction _chosenFunc;

public:
    OptionChooser(String choice1, String choice2, int activeItemIndex, eventAction chosenFunc);
    OptionChooser(String choice1, String choice2, String choice3, int activeItemIndex, eventAction chosenFunc);

    void onDisplay(bool modal = false) override;
    void onSelect() override;
    bool onKeypressed(int key) override;
    String getSelected();
};

