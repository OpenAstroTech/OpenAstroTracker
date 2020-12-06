#pragma once

#include "stdint.h"

#define MS_AVAILABLE(modes, mode) modes & mode

/**
 * This is the abstract interface to be implemented by specific driver implementations.
 * The interface itself is kept to a minimum to provide easy extensions by new hardware.
 * All the parameters required for correct setup and operation of the actual driver should
 * be provided in its constructor (dependency injection).
 */
class Driver
{
public:
    /**
     * Perform required preparations (e.g. setting up UART connection, max speed,
     * microstepping etc.)
     */
    virtual void setup() const = 0;

    /**
     * Move the motor at the specified speed (steps per second).  
     * Negative speed value leads to reversed direction.
     * Speed 0 leads to a stop of the motor.
     * limit defines the amount of steps to move at the specified speed.
     * maxSteps 0 lead to infinite movement (until another call of this function).
     */
    virtual void move(float speed, int microstepping = 1, float limit = 0) const = 0;

    /**
     * Return degrees per step in full step mode of the attached stepper motor.
     */
    virtual float getDegPerStep() const = 0;

    /**
     * Return available microstepping modes as bitmask of 16 bits. Modes are 
     * represented from lowest to highest bit (1,2,4,8,16,32,64,128,256...) respectively.
     * E.g. 0000_0000_0000_1011 stands for available modes: full step, half step and 8.
     * If microstepping is not supported, this function will return value 1.
     */
    virtual uint16_t getAvailableMicrosteppingModes() const = 0;

    /**
     * Return maximal speed (steps per second).
     */
    virtual uint16_t getMaxSpeed() const = 0;
};

// /**
//  * Abstract factory interface which should be implemented by the actual specific driver
//  * implementation. Its purpose is to keep all the driver specific code
//  * (even its initialization) together with the actual driver implementation. This way
//  * the higher level hardware abstraction can be kept clean and small.
//  */
    // class DriverFactory
    // {
    // public:
    //     /**
    //      * Create a new instance of the specific driver implementation in the heap memory
    //      * and return a pointer to it upcasted to the abstract Driver type. This way driver
    //      * users wont need to know the actual impementation and can use the driver in a
    //      * unified way.
    //      */
    //     virtual Driver* createInstance();
    // };
