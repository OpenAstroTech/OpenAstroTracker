#include "lib/util/debug.hpp"
#include "lib/util/utils.hpp"

#include "lib/input/LcdButtons.hpp"
#include "lib/menu/controls/MainMenu.hpp"
#include "OatPitchRollDisplay.hpp"
#include "Gyro.hpp"
#include "Mount.hpp"

PitchRollDisplay::PitchRollDisplay(String pitchOrRoll) : MenuItem("Roll Offset", pitchOrRoll)
{
}

bool PitchRollDisplay::onKeypressed(int key)
{
    if (key == btnLEFT)
    {
        getMainMenu()->closeMenuItem(this);
    }

    return MenuItem::onKeypressed(key);
}

bool PitchRollDisplay::onPreviewKey(int keyState)
{
    // Ignore Up and Down and Right buttons.
    if (keyState == btnUP)
        return true;
    if (keyState == btnDOWN)
        return true;
    if (keyState == btnRIGHT)
        return true;

    return MenuItem::onPreviewKey(keyState);
}

void PitchRollDisplay::onSelect()
{
    MenuItem::onSelect();
}

void PitchRollDisplay::onDisplay(bool modal)
{
    auto mount = Mount::instance();
    if (!_gyroStarted)
    {
        _pitchCalibrationAngle = mount->getPitchCalibrationAngle();
        _rollCalibrationAngle = mount->getRollCalibrationAngle();
        Gyro::startup();
    }

    char scratchBuffer[20];
    auto angles = Gyro::getCurrentAngles();
    if (_tag.equalsIgnoreCase("ROLL"))
    {
        sprintf(scratchBuffer, "R: -------------");
        makeIndicator(scratchBuffer, angles.rollAngle - _rollCalibrationAngle);
    }
    else
    {
        sprintf(scratchBuffer, "P: -------------");
        makeIndicator(scratchBuffer, angles.pitchAngle - _pitchCalibrationAngle);
    }

    getMainMenu()->writeToLCD(0, 1, scratchBuffer);
}

void PitchRollDisplay::makeIndicator(char *scratchBuffer, float angle)
{
    angle = clamp(angle, -9.9f, 9.9f);
    dtostrf(fabs(angle), 3, 1, &scratchBuffer[8]);
    for (int i = 0; i < 5; i++)
    {
        scratchBuffer[3 + i] = '-';
        scratchBuffer[11 + i] = '-';
    }

    int pos = clamp((int)round(angle * 4), -5, 5);
    if (pos != 0)
    {
        scratchBuffer[pos + ((pos < 0) ? 8 : 10)] = (pos < 0) ? '>' : '<';
    }
}
