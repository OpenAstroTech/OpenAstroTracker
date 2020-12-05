#pragma once

#include "../Configuration_adv.hpp"

#if USE_GYRO_LEVEL == 1
struct angle_t { float pitchAngle = 0; float rollAngle = 0; };

class Gyro 
{
public:
  static void startup();
  static void shutdown();
  static angle_t getCurrentAngles();
  static float getCurrentTemperature();
  
private:
  static bool isPresent;  // True if gyro correctly detected on startup
};
#endif
