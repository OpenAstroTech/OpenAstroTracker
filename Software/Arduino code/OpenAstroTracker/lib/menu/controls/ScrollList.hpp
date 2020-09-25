#pragma once

#include "MenuItem.hpp"

class ScrollList : public MenuItem
{
public:
    ScrollList();
    void onSelect() override;
    bool onKeypressed(int key) override;
};
