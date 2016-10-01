
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
 * stop dispatching messages but continue sleeping remainder of duration, with radio powerOff
 * (When dispatchQueuedMsg returns true, radio might be off.) ???
 *
 * Returns no earlier than time of call plus duration.
 * Might return slightly later if msg dispatch takes too long.
 *
 * Ensure message queue is nearly empty on return.
 * Could be a race to empty message queue.
 */
bool SyncAgent::dispatchMsgUntil(
		bool (*dispatchQueuedMsg)(), // function to dispatch a message, knows desired msg type
		OSTime (*timeoutFunc)())	// function returning remaining duration of slot
{
	bool didReceiveDesiredMsg = false;

	assert(!radio->isDisabled());	// is receiving

	while (true) {
		sleeper.sleepUntilEventWithTimeout(timeoutFunc());
		// dispatch on reason for wake
		if (sleeper.reasonForWakeIsMsgReceived()) {
			// call dispatcher
			didReceiveDesiredMsg = dispatchQueuedMsg();
			if (didReceiveDesiredMsg) {
				// Ultra low power sleep remainder of duration (radio power off)
				// assert radio not receiving, but still powered on
				radio->powerOff();
				// TODO but work slot requires it on?
				// Continue to next iteration i.e. sleep
				// assert since radio power off, reason for wake can only be timeout and will then exit loop
			}
			else {
				/*
				 * Dispatched message was not of desired type (but we could have done work, or other state changes.)
				 * restart receiver, remain in loop, sleep until next message
				 */
				radio->receiveStatic();
				// continuation is sleep
			}
			// assert msg queue is empty (since we received and didn't restart receiver)
		}
		else if (sleeper.reasonForWakeIsTimerExpired()) {
			// Slot done.
			// assert msg queue empty, except for race between timeout and receiver
			break;
		}
		else {
			// Unexpected wake from faults or brownout?
			// Brownout and bus faults (DMA?) could come while mcu is sleeping.
			// Invalid op code faults can not come while mcu is sleeping.
			// FUTURE recover from unexpected events by continuing?
			assert(false);
		}
	}
	// radio is on or off
	// ensure message queue nearly empty
	// ensure time elapsed
	return didReceiveDesiredMsg;
}

