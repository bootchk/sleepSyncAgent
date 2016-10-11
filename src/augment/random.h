
#pragma once

#include <inttypes.h>

// Wrapper to platform provided std lib functions

// Get a random integer in the range [min, max] including both end points.
// depends on stdlib rand()
uint16_t randUnsignedInt16(uint16_t min, uint16_t max);

// Random flip of fair coin
// depends on rand()
bool randBool();
