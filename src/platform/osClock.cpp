
#include "../config.h"
#include "osClock.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// Stub
OSTime OSClock::ticks() { return 1; }

#endif
//else platform provides
