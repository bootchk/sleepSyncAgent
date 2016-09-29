
#include <cassert>

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
 * On dispatchQueuedMsg() returns true (finds a desired message type),
 * stop dispatching messages but continue sleeping remainder of duration.
 * (When dispatchQueuedMsg returns true, radio might be off.)
 *
 * Returns no earlier than time of call plus duration.
 * Might return slightly later if msg dispatch takes too long.
 *
 * Ensure message queue is nearly empty on return.
 * Could be a race to empty message queue.
 */
void SyncAgent::dispatchMsgUntil(
		bool (*dispatchQueuedMsg)(), // function to dispatch a message
		OSTime (*timeoutFunc)())	// function returning remaining duration
{
	assert(!radio->isDisabled());	// is receiving
	while (true) {
		sleeper.sleepUntilEventWithTimeout(timeoutFunc());
		// reason for wake
		if (sleeper.reasonForWakeIsMsgReceived()) {
			if (dispatchQueuedMsg()) {
				// sleep remainder of duration
				// assert radio still on, more messages
				// TODO this is not right, recurse??
				sleeper.sleepUntilEventWithTimeout(timeoutFunc());
				break;
			}
			// TODO what does else mean?
			// assert msg queue is empty except for race
		}
		else if (sleeper.reasonForWakeIsTimerExpired()) {
			// msg queue empty
			break;
		}
		else {
			// Unexpected wake from faults or brownout?
			// Brownout and bus faults (DMA?) could come while mcu is sleeping.
			// Invalid op code faults can not come while mcu is sleeping.
			// TODO recover from unexpected events by continuing?
			assert(false);
		}
	}
	// radio is on or off
	// ensure message queue nearly empty
	// ensure time elapsed
}

