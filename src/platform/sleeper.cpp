#include "../../config.h"
#include "sleeper.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// stubs
void Sleeper::init();
void Sleeper::sleepUntilEventWithTimeout(OSTime) {}

bool Sleeper::reasonForWakeIsMsgReceived() { return false; }
bool Sleeper::reasonForWakeIsTimerExpired() { return true; }

void msgReceivedCallback() {}

#endif
