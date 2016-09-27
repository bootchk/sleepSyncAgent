
#include "../../config.h"
#include "tickCounter.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// Stub
OSTime OSClockTicks() { return 1; }

#endif
//else platform provides
