#include <Arduino.h>

#include "MenuItem.hpp"

typedef bool (*boolFunc)();

class ActionRunnerModal : public MenuItem
{
protected:
    MenuItem* _followModal;
    boolFunc _isComplete;

public:
    ActionRunnerModal(String displayname, String tag, MenuItem* followModal = nullptr);
    ActionRunnerModal(String displayname, String tag, boolFunc isComplete, MenuItem* followModal = nullptr);
    virtual bool isComplete() { return true; }
    void onDisplay(bool modal = false) override;
};

