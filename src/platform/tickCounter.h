
#pragma once

#include <inttypes.h>

#include "types.h"


// time that platform accepts for timeouts, scheduling
typedef uint32_t DeltaTime;
// NRF accepts 32 bits but only 24-bits valid

// The size of the OSClock in bits.  Used in LongClock.
const uint8_t OSClockCountBits = 24;
const uint32_t MaxDeltaTime = 0xFFFFFF;	// 24-bits

/*
 * Now time of platform's free-running, circular clock.
 *
 * Choosing the platform's more accurate clock is better for synchronization algorithm.
 */
OSTime OSClockTicks();
