#include "lib/menu/controls/MainMenu.hpp"
#include "lib/menu/controls/MenuItem.hpp"
#include "lib/menu/controls/ScrollList.hpp"
#include "lib/menu/controls/Button.hpp"
#include "lib/menu/controls/NumberInput.hpp"
#include "lib/menu/controls/OptionChooser.hpp"
#include "lib/menu/controls/MultiStepActionRunnerModal.hpp"

#include "configuration.hpp"
#include "StringTable.hpp"
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
        LOGV2(DEBUG_ANY, F("DA: Starting at %d"), driftStart);
        mount->stopSlewing(TRACKING);
        arg->setDisplay(oatString(UI_PAUSE_15SEC));
    }
    break;

    case ActionRunnerEventArgs::StepState::Running:
    {
        LOGV2(DEBUG_ANY, F("DA: Running. Step %d"), arg->getStep());
        switch (arg->getStep())
        {
        case 0:
        {
            if (millis() - driftStart > 1500)
            {
                LOGV1(DEBUG_ANY, F("DA: Wait over"));
                arg->setResult(ActionRunnerEventArgs::StepStateChange::Proceed);
                driftStart = millis();
                arg->setDisplay(oatString(UI_EASTWARD_PASS));
            }
        }
        break;
        case 1:
        {
            mount->runDriftAlignmentPhase(EAST, driftDuration);
            LOGV1(DEBUG_ANY, F("DA: East pass done"));
            arg->setResult(ActionRunnerEventArgs::StepStateChange::Proceed);
            driftStart = millis();
            arg->setDisplay(oatString(UI_PAUSE_15SEC));
        }
        break;
        case 2:
        {
            if (millis() - driftStart > 1500)
            {
                LOGV1(DEBUG_ANY, F("DA: Wait2 over"));
                arg->setResult(ActionRunnerEventArgs::StepStateChange::Proceed);
                driftStart = millis();
                arg->setDisplay(oatString(UI_WESTWARD_PASS));
            }
        }
        break;
        case 3:
        {
            mount->runDriftAlignmentPhase(WEST, driftDuration);
            LOGV1(DEBUG_ANY, F("DA: West pass done"));
            arg->setResult(ActionRunnerEventArgs::StepStateChange::Proceed);
            driftStart = millis();
            arg->setDisplay(oatString(UI_PAUSE_15SEC));
        }
        break;
        case 4:
        {
            if (millis() - driftStart > 1500)
            {
                arg->setDisplay(oatString(UI_RESET_MOUNT));
                mount->runDriftAlignmentPhase(0, 0);
                arg->setResult(ActionRunnerEventArgs::StepStateChange::Proceed);
            }
        }
        break;
        case 5:
        {
            mount->startSlewing(TRACKING);
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

    args->getSource()->getMainMenu()->writeToLCD(0, 1, F("Aligned, homing..."));
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
    if (arg->getState() == ActionRunnerEventArgs::StepState::Starting)
    {
        // Move the RA to that of Polaris. Moving to this RA aligns the DEC axis such that
        // it swings along the line between Polaris and the Celestial Pole.
        mount->targetRA() = DayTime(PolarisRAHour, PolarisRAMinute, PolarisRASecond);

        // Set DEC to move the same distance past Polaris as
        // it is from the Celestial Pole. That equates to 88deg 42' 11.2".
        mount->targetDEC() = DegreeTime(88 - (NORTHERN_HEMISPHERE ? 90 : -90), 42, 11);
        mount->startSlewingToTarget();
        arg->getSource()->getMainMenu()->activateDialog(oatString(DLG_SLEW_DISPLAY));
    }

    if (arg->getState() == ActionRunnerEventArgs::StepState::Running)
    {
        if (!mount->isSlewingRAorDEC())
        {
            arg->setResult(ActionRunnerEventArgs::StepStateChange::Completed);
            arg->getSource()->getMainMenu()->writeToLCD(0, 1, oatString(UI_ADJUST_MOUNT));
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

MenuItem dlgPaStoreSync(oatString(UI_ADJUST_MOUNT), oatString(STORE_AND_SYNC));
Button calPaStoreBtn(oatString(UI_CENTERED), &paStoreAndSync);
MultiStepActionRunnerModal dlgPaSlewToPolaris(oatString(UI_SLEWING), oatString(SLEW_TO_POLARIS), &paSlewingProgress, &dlgPaStoreSync);
Button calStartPaBtn(oatString(UI_POLAR_ALIGNMENT), &paActivateSlewToPolaris);

OptionChooser driftLenOption("1m", "2m", "3m", "5m", 0, &setDriftLength);
Button driftAlignBtn(oatString(DRIFT_ALIGNMENT), &driftLenOption);
MultiStepActionRunnerModal dlgDriftAlign(oatString(DRIFT_ALIGNMENT), oatString(DRIFT_ALIGNMENT), &driftAlignPhaseFunction, nullptr);

NumberInput speedCalNum("SPD", 1, oatString(UI_SPEED_FACTOR), nullptr, &speedIncr, NumberInput::BehaviorFlags::AcceleratingRepetition);
Button calSpeedCal(oatString(UI_SPEED_CALIBRATION), &speedCalNum);
NumberInput raStepNum("RA", 1, oatString(UI_RA_STEPS), nullptr, &speedIncr, NumberInput::BehaviorFlags::AcceleratingRepetition);
Button calRaSteps(oatString(UI_RA_STEP_ADJUST), &raStepNum);
NumberInput decStepNum("DEC", 1, oatString(UI_DEC_STEPS), nullptr, &speedIncr, NumberInput::BehaviorFlags::AcceleratingRepetition);
Button calDecSteps(oatString(UI_DEC_STEP_ADJUST), &decStepNum);
NumberInput backlashNum("BACK", 1, oatString(UI_BACKLASH), nullptr, &speedIncr, NumberInput::BehaviorFlags::AcceleratingRepetition);
Button calBacklash(oatString(UI_BACKLASH_ADJUST), &backlashNum);
PitchRollDisplay calRoll("ROLL");
Button calRollOffset(oatString(UI_ROLL_OFFSET), &calRoll);
PitchRollDisplay calPitch("PITCH");
Button calPitchOffset(oatString(UI_PITCH_OFFSET), &calPitch);

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
    mainMenu.addModalDialog(&dlgDriftAlign);
}
