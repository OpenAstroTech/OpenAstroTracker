#pragma once

#include "Configuration_adv.hpp"

#if USE_GYRO_LEVEL == 1
struct angle_t { float pitchAngle; float rollAngle; };

class Gyro 
{
public:
  static void startup();
  static void shutdown();
  static angle_t getCurrentAngles();
  static float getCurrentTemperature();
  
private:
  static int16_t AcX, AcY, AcZ;
  static int16_t AcXOffset, AcYOffset, AcZOffset;
};
#endif