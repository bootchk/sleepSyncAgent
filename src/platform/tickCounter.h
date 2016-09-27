
#include <inttypes.h>

#include "types.h"

/*
 * Now time of platform's free-running, circular clock.
 *
 * Choosing the platform's more accurate clock is better for synchronization algorithm.
 */
OSTime OSClockTicks();
