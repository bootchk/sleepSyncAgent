#include "../../config.h"
#include "sleep.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// stubs

void sleepUntilEventWithTimeout(OSTime) {}

bool reasonForWakeIsMsgReceived() { return false; }
bool reasonForWakeIsTimerExpired() { return true; }

#endif
