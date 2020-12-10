#pragma once

#include <Arduino.h>
#include "DayTime.hpp"

// 90 at north pole, -90 at south pole
class Latitude : public DayTime
{
public:
  Latitude() : DayTime() {}
  Latitude(const Latitude &other);
  Latitude(int h, int m, int s);
  Latitude(float inDegrees);

  static Latitude ParseFromMeade(String s);

protected:
  virtual void checkHours() override;
};
