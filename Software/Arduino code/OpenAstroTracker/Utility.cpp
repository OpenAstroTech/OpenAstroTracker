#include <Arduino.h>
#include "Globals.h"

// Adjust the given number by the given adjustment, wrap around the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
int adjustWrap(int current, int adjustBy, int minVal, int maxVal)
{
  current += adjustBy;
  if (current > maxVal) current -= (maxVal + 1 - minVal);
  if (current < minVal) current += (maxVal + 1 - minVal);
  return current;
}

// Adjust the given number by the given adjustment, clamping to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
int adjustClamp(int current, int adjustBy, int minVal, int maxVal)
{
  current += adjustBy;
  if (current > maxVal) current = maxVal;
  if (current < minVal) current = minVal;
  return current;
}

// Clamp the given number to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
long clamp(long current, long minVal, long maxVal)
{
  if (current > maxVal) current = maxVal;
  if (current < minVal) current = minVal;
  return current;
}

// Clamp the given number to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
float clamp(float current, float minVal, float maxVal)
{
  if (current > maxVal) current = maxVal;
  if (current < minVal) current = minVal;
  return current;
}
/*
int read_LCD_buttons() {
  adc_key_in = analogRead(0);
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 240)  return btnUP;
  if (adc_key_in < 400)  return btnDOWN;
  if (adc_key_in < 600)  return btnLEFT;
  if (adc_key_in < 920)  return btnSELECT;
}
*/
