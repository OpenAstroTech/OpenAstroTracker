#include "Configuration.hpp"
#include "lib/util/utils.hpp"
#include "lib/menu/controls/MainMenu.hpp"
#include "speedIncrementer.hpp"
#include "Mount.hpp"

SpeedAndStepIncrementer::SpeedAndStepIncrementer() : Incrementer()
{
}

void SpeedAndStepIncrementer::readValues()
{
    auto mount = Mount::instance();
    float speed = mount->getSpeedCalibration() - 1.0f;
    _speed = (int)(speed * 10000.0f);

    _raSteps = mount->getStepsPerDegree(RA_STEPS);
    _decSteps = mount->getStepsPerDegree(DEC_STEPS);
    _backlashSteps = mount->getBacklashCorrection();
}

void SpeedAndStepIncrementer::onChange(String tag, int *numbers, int index, int val)
{
    Incrementer::onChange(tag, numbers, index, val);
    if (tag.equals("SPD"))
    {
        _speed = numbers[0];
    }
    else if (tag.equals("RA"))
    {
        _raSteps = numbers[0];
    }
    else if (tag.equals("DEC"))
    {
        _decSteps = numbers[0];
    }
    else if (tag.equals("BACK"))
    {
        _backlashSteps = numbers[0];
    }
}

String SpeedAndStepIncrementer::getDisplay(String tag, int index, int val, String formatString)
{
    if (tag.equals("SPD"))
    {
        String(1.0 + 1.0 * val / 10000.0, 4);
    }

    return String(val);
}

void SpeedAndStepIncrementer::getNumbers(String tag, int *numbers)
{
    if (tag.equals("SPD"))
    {
        numbers[0] = _speed;
    }
    else if (tag.equals("RA"))
    {
        numbers[0] = _raSteps;
    }
    else if (tag.equals("DEC"))
    {
        numbers[0] = _decSteps;
    }
    else if (tag.equals("BACK"))
    {
        numbers[0] = _backlashSteps;
    }
}

void SpeedAndStepIncrementer::onSelect(EventArgs *args)
{
    auto mount = Mount::instance();
    if (args->getSource()->getTag().equals("SPD"))
    {
        mount->setSpeedCalibration(1.0 + _speed / 10000.0, true);
        args->getSource()->getMainMenu()->writeToLCD(0, 1, "Speed stored.");
        mount->delay(500);
    }
    else if (args->getSource()->getTag().equals("RA"))
    {
        mount->setStepsPerDegree(RA_STEPS, _raSteps);
        args->getSource()->getMainMenu()->writeToLCD(0, 1, "RA steps stored.");
        mount->delay(500);
    }
    else if (args->getSource()->getTag().equals("DEC"))
    {
        mount->setStepsPerDegree(DEC_STEPS, _decSteps);
        args->getSource()->getMainMenu()->writeToLCD(0, 1, "DEC steps stored.");
        mount->delay(500);
    }
    else if (args->getSource()->getTag().equals("BACK"))
    {
        mount->setBacklashCorrection(_backlashSteps);
        args->getSource()->getMainMenu()->writeToLCD(0, 1, "Backlash stored.");
        mount->delay(500);
    }
}
