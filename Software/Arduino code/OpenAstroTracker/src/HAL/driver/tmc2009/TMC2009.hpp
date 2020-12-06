#pragma once

#include "../Driver.hpp"

class TMC2009 : public Driver
{
private:
    uint16_t _maxSpeed;
public:
    TMC2009(
        uint16_t maxSpeed
    );

    uint16_t getMaxSpeed() const override; 
};
