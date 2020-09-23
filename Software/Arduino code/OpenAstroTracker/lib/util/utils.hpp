#pragma once

// Adjust the given number by the given adjustment, wrap around the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
int adjustWrap(int current, int adjustBy, int minVal, int maxVal);

// Adjust the given number by the given adjustment, clamping to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
int adjustClamp(int current, int adjustBy, int minVal, int maxVal);

// Clamp the given number to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
long clamp(long current, long minVal, long maxVal);

// Clamp the given number to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
int clamp(int current, int minVal, int maxVal);

// Clamp the given number to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
float clamp(float current, float minVal, float maxVal);
