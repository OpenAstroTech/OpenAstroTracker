#pragma once

#include "lib/menu/incrementers/incrementer.hpp"
#include "Utility.hpp"
#include "Mount.hpp"

class RaDecIncrementer : public Incrementer
{
    bool _isRA;
    int showingTarget;
public:
    RaDecIncrementer(String which);
    void onChange(String tag, int *numbers, int index, int val) override;
    String getDisplay(String tag, int index, int val, String formatString) override;
    void getNumbers(String tag, int *numbers) override;
};

