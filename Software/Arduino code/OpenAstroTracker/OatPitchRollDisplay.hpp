#include <Arduino.h>

#include "lib/menu/controls/MenuItem.hpp"

class PitchRollDisplay : public MenuItem
{
private:
    float _pitchCalibrationAngle;
    float _rollCalibrationAngle;
    bool _gyroStarted;

public:
    PitchRollDisplay(String tag);
    void onDisplay(bool modal = false) override;
    void onSelect() override;
    bool onKeypressed(int key) override;
    bool onPreviewKey(int keyState) override;

private:
    void makeIndicator(char *scratchBuffer, float angle);
};
