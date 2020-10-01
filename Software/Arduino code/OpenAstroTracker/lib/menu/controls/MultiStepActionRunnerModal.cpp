#include "../../util/debug.hpp"
#include "../../util/utils.hpp"
#include "MainMenu.hpp"
#include "MultiStepActionRunnerModal.hpp"

MultiStepActionRunnerModal::MultiStepActionRunnerModal(String displayname, String tag, runnerFunc stateHandler, MenuItem *followModal) : ActionRunnerModal(displayname, tag, followModal)
{
    _stateHandler = stateHandler;
}

bool MultiStepActionRunnerModal::isComplete()
{
    return _completed;
}

void MultiStepActionRunnerModal::onDisplay(bool modal)
{
    auto args = new ActionRunnerEventArgs(this, _steps, ActionRunnerEventArgs::StepState::Starting);
    if (_steps == -1)
    {
        _steps = 0;
        _stateHandler(args);
        if (args->getResult() == ActionRunnerEventArgs::StepStateChange::Completed)
        {
            _completed = true;
        }
    }

    args->setState(ActionRunnerEventArgs::StepState::Running);
    args->setResult(ActionRunnerEventArgs::StepStateChange::NoChange);
    args->setHeading(getDisplayName());

    // Let the user tell us what to do
    _stateHandler(args);

    if (args->getResult() == ActionRunnerEventArgs::StepStateChange::Completed)
    {
        _completed = true;
    }
    else if (args->getResult() == ActionRunnerEventArgs::StepStateChange::Backtrack)
    {
        _steps = max(_steps - 1, 0);
    }
    else if (args->getResult() == ActionRunnerEventArgs::StepStateChange::Proceed)
    {
        _steps++;
    }

    if (args->getHeading().length() > 0)
    {
        getMainMenu()->writeToLCD(0, 0, args->getHeading());
    }
    if (args->getDisplay().length() > 0)
    {
        getMainMenu()->writeToLCD(0, 1, args->getDisplay());
    }

    ActionRunnerModal::onDisplay(modal);
}
