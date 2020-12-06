#include "TMC2009.hpp"

TMC2009::TMC2009(uint16_t maxSpeed) : _maxSpeed(maxSpeed)
{
}

uint16_t TMC2009::getMaxSpeed() const
{
    return _maxSpeed;
}