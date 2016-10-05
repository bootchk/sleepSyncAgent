#include "../../config.h"
#include "sleeper.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// stubs
void Sleeper::init() {}
bool Sleeper::isOSClockRunning() { return true; }
void Sleeper::sleepUntilEventWithTimeout(OSTime) {}

void Sleeper::clearReasonForWake() {}

bool Sleeper::reasonForWakeIsMsgReceived() { return false; }
bool Sleeper::reasonForWakeIsTimerExpired() { return true; }
bool Sleeper::reasonForWakeIsCleared() { return true; }

void Sleeper::msgReceivedCallback() {}

#endif
