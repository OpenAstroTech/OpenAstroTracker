#include "RotationAxis.hpp"

RotationAxis::RotationAxis(Driver *driver) : _driver(driver) {}

void RotationAxis::setup()
{
    _driver->setup();
}

void RotationAxis::rotate(float speed, float limit)
{
    /* 
    * perform conversion from arcsecs to steps. Take in account:
    * - stepper degs per step
    * - reduction (ring radius, pulley etc.)
    */
}