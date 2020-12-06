#pragma once

#include "../driver/Driver.hpp"

/**
 * Rotation axis used for most movements of the tracking mount. The axis class calculates
 * conversions from arcsecs to steps of its stepper taking into account gear reductions, 
 * max speed, stepper and driver characteristics etc. It should be used by the mount without
 * exposing detailed information about the used hardware.
 **/
class RotationAxis
{
private:
    Driver* _driver;
public:

    /**
     * Construct a new RotationAxis instance.
     * driver - pointer to a specific implementation instance of the Driver interface.
     **/
    RotationAxis(Driver* driver);

    /**
     * Perform initial setup of this axis. This calling setup of the motor driver.
     **/
    void setup();

    /**
     * Rotate the axis at the specified speed.
     * speed - arcsecs per second. Negative for reversed direction. Zero for stop.
     * limit - limit the motion to specific amount of arcseconds. Zero (default) disables
     * the limit and the axis will rotate indifinitely.
     **/
    void rotate(float speed, float limit = 0.0f);
};
