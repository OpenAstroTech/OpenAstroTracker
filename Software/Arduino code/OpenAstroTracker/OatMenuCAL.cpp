#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/ScrollList.hpp"
#include "lib/menu/controls/Button.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "lib/menu/controls/OptionChooser.hpp"
#include "lib/menu/controls/MultiStepActionRunnerModal.hpp"

#include "Strings.hpp"
#include "speedIncrementer.hpp"
#include "OatPitchRollDisplay.hpp"
#include "Mount.hpp"

void paActivateSlewToPolaris(EventArgs *arg)
{
    arg->getSource()->getMainMenu()->activateDialog(oatString(SLEW_TO_POLARIS));
}

void activateDriftAlign(EventArgs *arg)
{
    arg->getSource()->getMainMenu()->activateDialog(oatString(DRIFT_ALIGNMENT));
}

unsigned long driftStart = 0;
unsigned long driftDuration = 0;
int driftDurationTimes[] = {27, 57, 87, 147};

void driftAlignPhaseFunction(ActionRunnerEventArgs *arg)
{
    auto mount = Mount::instance();
    arg->setHeading(oatString(DRIFT_ALIGNMENT));
    switch (arg->getState())
    {
    case ActionRunnerEventArgs::StepState::Starting:
    {
        driftStart = millis();
    }
    break;

    case ActionRunnerEventArgs::StepState::Running:
    {
        switch (arg->getStep())
        {
        case 0:
        {
            arg->setDisplay(oatString(UI_PAUSE_15SEC));
            if (millis() - driftStart > 1500)
            {
                arg->setResult(ActionRunnerEventArgs::StepStateChange::Proceed);
                driftStart = millis();
            }
        }
        break;
        case 1:
        {
            arg->setDisplay(oatString(UI_EASTWARD_PASS));
            if (millis() - driftStart > driftDuration)
            {
                arg->setResult(ActionRunnerEventArgs::StepStateChange::Proceed);
                driftStart = millis();
            }
        }
        break;
        case 2:
        {
            arg->setDisplay(oatString(UI_PAUSE_15SEC));
            if (millis() - driftStart > 1500)
            {
                arg->setResult(ActionRunnerEventArgs::StepStateChange::Proceed);
                driftStart = millis();
            }
        }
        break;
        case 3:
        {
            arg->setDisplay(oatString(UI_WESTWARD_PASS));
            if (millis() - driftStart > driftDuration)
            {
                arg->setResult(ActionRunnerEventArgs::StepStateChange::Proceed);
                driftStart = millis();
            }
        }
        break;
        case 4:
        {
            arg->setDisplay(oatString(UI_PAUSE_15SEC));
            if (millis() - driftStart > 1500)
            {
                arg->setResult(ActionRunnerEventArgs::StepStateChange::Proceed);
                driftStart = millis();
            }
        }
        break;
        case 5:
        {
            arg->setDisplay(oatString(UI_RESET_MOUNT));
            arg->setResult(ActionRunnerEventArgs::StepStateChange::Completed);
        }
        break;
        }
    }
    break;
    }
}

void paStoreAndSync(EventArgs *args)
{
    auto mount = Mount::instance();

    args->getSource()->getMainMenu()->writeToLCD(0, 1, "Aligned, homing...");
    mount->delay(750);

    // Sync the mount to Polaris, since that's where it's pointing
    DayTime currentRa = mount->currentRA();
    mount->syncPosition(currentRa.getHours(), currentRa.getMinutes(), currentRa.getSeconds(), 89 - (NORTHERN_HEMISPHERE ? 90 : -90), 21, 6);

    // Go home from here
    mount->setTargetToHome();
    mount->startSlewingToTarget();

    args->getSource()->getMainMenu()->closeDialog();
}

void paSlewingProgress(ActionRunnerEventArgs *arg)
{
    auto mount = Mount::instance();
    if (arg->getState() == ActionRunnerEventArgs::StepState::Running)
    {
        arg->setDisplay(oatString(UI_SLEWING));
        if (!mount->isSlewingRAorDEC())
        {
            arg->setResult(ActionRunnerEventArgs::StepStateChange::Completed);
        }
    }
}

void setDriftLength(EventArgs *args)
{
    int index = ((OptionChooser *)args->getSource())->getSelectedIndex();
    driftDuration = driftDurationTimes[index];
    args->getSource()->getMainMenu()->activateDialog(oatString(DRIFT_ALIGNMENT));
}

SpeedAndStepIncrementer speedIncr;

MenuItem calMenu("CAL", "CAL");
ScrollList calList;

MenuItem dlgPaStoreSync("Adjust mount", "StoreAndSync");
Button calPaStoreBtn("Centered", &paStoreAndSync);
MultiStepActionRunnerModal dlgPaSlewToPolaris(oatString(UI_SLEWING), oatString(SLEW_TO_POLARIS), &paSlewingProgress, &dlgPaStoreSync);
Button calStartPaBtn("Polar Alignmnt", &paActivateSlewToPolaris);

OptionChooser driftLenOption("1m", "2m", "3m", "5m", 0, &setDriftLength);
Button driftAlignBtn(oatString(DRIFT_ALIGNMENT), &driftLenOption);
MultiStepActionRunnerModal dlgDriftAlign(oatString(DRIFT_ALIGNMENT), oatString(DRIFT_ALIGNMENT), &driftAlignPhaseFunction, nullptr);

NumberInput speedCalNum("SPD", 1, "SpdFctr: @", nullptr, &speedIncr, NumberInput::BehaviorFlags::AcceleratingRepetition);
Button calSpeedCal("Speed Calibratn", &speedCalNum);
NumberInput raStepNum("RA", 1, "RA Steps: @", nullptr, &speedIncr, NumberInput::BehaviorFlags::AcceleratingRepetition);
Button calRaSteps("RA Step Adjust", &raStepNum);
NumberInput decStepNum("DEC", 1, "DEC Steps: @", nullptr, &speedIncr, NumberInput::BehaviorFlags::AcceleratingRepetition);
Button calDecSteps("DEC Step Adjust", &decStepNum);
NumberInput backlashNum("BACK", 1, "Backlash: @", nullptr, &speedIncr, NumberInput::BehaviorFlags::AcceleratingRepetition);
Button calBacklash("Backlash Adjust", &backlashNum);
PitchRollDisplay calRoll("ROLL");
Button calRollOffset("Roll Offset", &calRoll);
PitchRollDisplay calPitch("PITCH");
Button calPitchOffset("Pitch Offset", &calPitch);

void createCALMenu(MainMenu &mainMenu)
{
    speedIncr.readValues();

    dlgPaStoreSync.addMenuItem(&calPaStoreBtn);

    calList.addMenuItem(&calStartPaBtn);
    calList.addMenuItem(&driftAlignBtn);
    calList.addMenuItem(&calSpeedCal);
    calList.addMenuItem(&calRaSteps);
    calList.addMenuItem(&calDecSteps);
    calList.addMenuItem(&calBacklash);
    calList.addMenuItem(&calRollOffset);
    calList.addMenuItem(&calPitchOffset);

    calMenu.addMenuItem(&calList);
    mainMenu.addMenuItem(&calMenu);

    mainMenu.addModalDialog(&dlgPaSlewToPolaris);
    mainMenu.addModalDialog(&dlgPaStoreSync);
}
