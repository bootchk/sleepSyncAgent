
#include <cassert>

#include "../platform/radio.h"
#include "../platform/mailbox.h"
#include "syncAgent.h"


/*
 * Sleep until duration with radio on.
 * Wake from sleep to dispatch messsages received.
 *
 * timeoutFunc gives remaining duration at time of call
 * (duration is known only to caller, but is generally fixed.)
 * dispatchQueuedMsg dispatches message on queue
 *
 * On dispatcher returns true (finds a desired message type),
 * stop dispatching messages but continue sleeping remainder of duration.
 * (When dispatcher returns true, radio might be off.)
 *
 * Returns no earlier than time of call plus duration.
 * Might return slightly later if msg dispatch takes too long.
 *
 * Ensure message queue is nearly empty on return.
 * Could be a race to empty message queue.
 */
void SyncAgent::dispatchMsgUntil(
		bool (*dispatchQueuedMsg)(),
		OSTime (*timeoutFunc)()) {
	assert(isReceiverOn());
	while (true) {
		sleepUntilEventWithTimeout(timeoutFunc());
		// reason for wake
		if (reasonForWakeIsMsgReceived()) {
			if (dispatchQueuedMsg()) {
				// sleep remainder of duration
				// assert radio still on, more messages
				sleepUntilEventWithTimeout(timeoutFunc());
				break;
			}
			// TODO what does else mean?
			// assert msg queue is empty except for race
		}
		else if (reasonForWakeIsTimerExpired()) {
			// msg queue empty
			break;
		}
		else {
			// Unexpected wake from faults or brownout?
			// TODO recover by continuing?
			assert(false);
		}
	}
	// radio is on or off
	// ensure message queue nearly empty
	// ensure time elapsed
}

