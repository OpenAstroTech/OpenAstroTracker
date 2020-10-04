#include "Configuration_adv.hpp"

#if USE_GYRO_LEVEL == 1
#include <Wire.h> // I2C communication library

#include "Utility.hpp"
#include "Gyro.hpp"

const int MPU = 0x68; // I2C address of the MPU6050 accelerometer

int16_t Gyro::AcX, Gyro::AcY, Gyro::AcZ;
int16_t Gyro::AcXOffset, Gyro::AcYOffset, Gyro::AcZOffset;

void Gyro::startup()
{
    // Initialize interface to the MPU6050
    LOGV1(DEBUG_INFO, F("GYRO:: Starting"));
    Wire.begin();
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);

    // read the values 100 times for them to stabilize
    for (int i = 0; i < 100; i++)
    {
        Wire.beginTransmission(MPU);
        Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
        Wire.endTransmission(false);
        Wire.requestFrom(MPU, 6, true);       // Read 6 registers total, each axis value is stored in 2 registers
        AcX = Wire.read() << 8 | Wire.read(); // X-axis value
        AcY = Wire.read() << 8 | Wire.read(); // Y-axis value
        AcZ = Wire.read() << 8 | Wire.read(); // Z-axis value
    }
    LOGV1(DEBUG_INFO, F("GYRO:: Started"));
}

void Gyro::shutdown()
{
    LOGV1(DEBUG_INFO, F("GYRO: Shutdown"));
    Wire.end();
}

angle_t Gyro::getCurrentAngles()
{
    const int windowSize = 16;
    // Read the accelerometer data
    struct angle_t result;
    result.pitchAngle = 0;
    result.rollAngle = 0;
    for (int i = 0; i < windowSize; i++)
    {
        Wire.beginTransmission(MPU);
        Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
        Wire.endTransmission(false);
        Wire.requestFrom(MPU, 6, true);       // Read 6 registers total, each axis value is stored in 2 registers
        AcX = Wire.read() << 8 | Wire.read(); // X-axis value
        AcY = Wire.read() << 8 | Wire.read(); // Y-axis value
        AcZ = Wire.read() << 8 | Wire.read(); // Z-axis value

        // Calculating the Pitch angle (rotation around Y-axis)
        result.pitchAngle += ((atan(-1 * AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180.0 / PI) * 2.0) / 2.0;
        // Calculating the Roll angle (rotation around X-axis)
        result.rollAngle += ((atan(-1 * AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180.0 / PI) * 2.0) / 2.0;
    }

    result.pitchAngle /= windowSize;
    result.rollAngle /= windowSize;
#if GYRO_AXIS_SWAP == 1
    float temp = result.pitchAngle;
    result.pitchAngle = result.rollAngle;
    result.rollAngle = temp;
#endif
    return result;
}

float Gyro::getCurrentTemperature()
{
    // Read the temperature data
    float result = 0.0;
    int16_t tempValue;
    Wire.beginTransmission(MPU);
    Wire.write(0x41); // Start with register 0x41 (TEMP_OUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 2, true);             // Read 2 registers total, the temperature value is stored in 2 registers
    tempValue = Wire.read() << 8 | Wire.read(); // Raw Temperature value
    // Calculating the actual temperature value
    result = float(tempValue) / 340 + 36.53;
    return result;
}
#endif
