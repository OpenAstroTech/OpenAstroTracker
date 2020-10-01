#include "../../util/debug.hpp"
#include "MainMenu.hpp"
#include "ActionRunnerModal.hpp"

ActionRunnerModal::ActionRunnerModal(String displayname, String tag, MenuItem *followModal) : MenuItem(displayname, tag)
{
    _isComplete = nullptr;
    _followModal = followModal;
}

ActionRunnerModal::ActionRunnerModal(String displayname, String tag, boolFunc isComplete, MenuItem *followModal) : MenuItem(displayname, tag)
{
    _isComplete = isComplete;
    _followModal = followModal;
}

void ActionRunnerModal::onDisplay(bool modal)
{
    bool complete = _isComplete == nullptr ? isComplete() : _isComplete();
    if (complete)
    {
        if (_followModal != nullptr)
        {
            LOGV2(255, F("ARM: Modal action is complete, activating next dialog %s "), _followModal->getTag().c_str());
            getMainMenu()->activateDialog(_followModal->getTag());
        }
        else
        {
            LOGV1(255, F("ARM: Modal action is complete, closing dialog."));
            getMainMenu()->closeDialog();
        }
    }
}
