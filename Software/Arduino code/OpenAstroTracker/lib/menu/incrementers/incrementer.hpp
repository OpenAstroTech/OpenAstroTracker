#pragma once
#include <Arduino.h>
#include "../controls/EventArgs.hpp"

const int INCREMENT_NO_LIMITS = 0;
const int INCREMENT_WRAP = 1;
const int INCREMENT_CLAMP = 2;

class Incrementer
{
private:
    int _mode;
    int *_maxVals;
    int *_minVals;

public:
    Incrementer(int mode = INCREMENT_NO_LIMITS, int *maxVals = nullptr, int *minVals = nullptr);
    virtual void onChange(String tag, int* numbers, int index, int val);
    virtual String getDisplay(String tag, int index, int val, String formatString = "");
    virtual void getNumbers(String tag, int* nums) {};
    virtual void onSelect(EventArgs *args) {};

protected:
    void setMinVals(int* minVals);
    void setMaxVals(int* maxVals);
};

