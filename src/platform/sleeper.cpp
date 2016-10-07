#include "../../config.h"
#include "sleeper.h"

#ifndef SYNC_AGENT_IS_LIBRARY

// stubs
void Sleeper::init() {}
bool Sleeper::isOSClockRunning() { return true; }
void Sleeper::sleepUntilEventWithTimeout(OSTime) {}

ReasonForWake Sleeper::reasonForWake();
void Sleeper::clearReasonForWake() { return MsgReceived; }


void Sleeper::msgReceivedCallback() {}

#endif
