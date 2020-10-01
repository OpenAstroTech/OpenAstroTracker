#pragma once

#include "../../util/debug.hpp"
#include "ActionRunnerModal.hpp"
#include "ActionRunnerEventArgs.hpp"

typedef void (*runnerFunc)(ActionRunnerEventArgs *args);

class MultiStepActionRunnerModal : public ActionRunnerModal
{
private:
    int _steps = -1;
    bool _completed = false;
    runnerFunc _stateHandler;

public:
    MultiStepActionRunnerModal(String displayname, String tag, runnerFunc stateHander, MenuItem *followModal);
    bool isComplete() override;
    void onDisplay(bool modal = false) override;
};
