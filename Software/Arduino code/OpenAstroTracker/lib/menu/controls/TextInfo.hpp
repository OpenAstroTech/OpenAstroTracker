#pragma once
#include "MenuItem.hpp"

typedef String (*displayFunc)();

class TextInfo : public MenuItem
{
private:
    String _prompt;
    displayFunc _displayFunc;

public:
    TextInfo(String prompt, displayFunc displayFunc);
    void onDisplay(bool modal = false) override;
};

