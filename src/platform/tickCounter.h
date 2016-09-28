
#include <inttypes.h>

#include "types.h"


// The size of the OSClock in bits.  Used in LongClock.
const uint8_t OSClockCountBits = 24;

/*
 * Now time of platform's free-running, circular clock.
 *
 * Choosing the platform's more accurate clock is better for synchronization algorithm.
 */
OSTime OSClockTicks();
