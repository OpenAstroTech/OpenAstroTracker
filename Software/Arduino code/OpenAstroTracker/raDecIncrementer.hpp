#pragma once

#include "lib/menu/incrementers/incrementer.hpp"
#include "Utility.hpp"
#include "Mount.hpp"

int minDec[] = {-90, 0, 0, 0};
int maxDec[] = {90, 59, 59, 1};
int minRa[] = {0, 0, 0, 0};
int maxRa[] = {23, 59, 59, 1};

class RaDecIncrementer : public Incrementer
{
    bool _isRA;
public:
    RaDecIncrementer(String which);
    void onChange(String tag, int *numbers, int index, int val) override;
    String getDisplay(String tag, int index, int val, String formatString) override;
    void getNumbers(String tag, int *numbers) override;
};

