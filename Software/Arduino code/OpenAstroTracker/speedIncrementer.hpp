#pragma once

#include "lib/menu/incrementers/incrementer.hpp"
#include "Utility.hpp"
#include "Mount.hpp"

class SpeedAndStepIncrementer : public Incrementer
{
private:
    int _speed = 0; 
    int _raSteps = 0;
    int _decSteps = 0;
    int _backlashSteps = 0;

public:
    SpeedAndStepIncrementer();
    void readValues();
    void onChange(String tag, int *numbers, int index, int val) override;
    String getDisplay(String tag, int index, int val, String formatString) override;
    void getNumbers(String tag, int *numbers) override;
    void onSelect(EventArgs *args) override;
};
