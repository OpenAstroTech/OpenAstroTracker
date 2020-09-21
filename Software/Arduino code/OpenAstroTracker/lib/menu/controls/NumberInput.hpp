#pragma once

#include <Arduino.h>
#include "MenuItem.hpp"

class NumberInput : public MenuItem
{
public:
    typedef enum BehaviorFlags
    {
        ConstantRepetition,
        AcceleratingRepetition
    };

private:
    String _mask;
    int _activeNumber;
    int *_numbers;
    int _numNumbers;
    eventAction _selectCallback;
    unsigned long _nextKeyOk;
    unsigned long _waitFor;
    BehaviorFlags _flags;
    bool _useIncrForDisplay;
    Incrementer* _incr;

public:
    NumberInput(String tag, int numNumbers, String mask, eventAction selectCallback, Incrementer* incr, BehaviorFlags flags = BehaviorFlags::ConstantRepetition);
    NumberInput(String tag, int* numbers, int numNumbers, String mask, eventAction selectCallback, Incrementer* incr = nullptr, BehaviorFlags flags = BehaviorFlags::ConstantRepetition);
    void onDisplay(bool modal = false) override;
    void onSelect() override;
    int getValue(int index);
    bool onPreviewKey(int keyState) override;
    bool onKeypressed(int key) override;
}
