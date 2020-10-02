#pragma once
#include "MenuItem.hpp"

typedef String (*multiDisplayFunc)(int index);

class MultiTextInfo : public MenuItem
{
private:
    int _currentSubItem;
    int _numSubItems;
    multiDisplayFunc _displayFunc;

public:
    MultiTextInfo(int numSubItems, multiDisplayFunc displayFunc);
    bool onKeypressed(int key) override;
    void onDisplay(bool modal = false) override;
};

