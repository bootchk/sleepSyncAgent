
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
 * Kind of desired message is different for each dispatcher,
 * and might also depend on a message and state?
 *
 * Returns no earlier than time of call plus duration.
 * Might return slightly later if msg dispatch takes too long.
 *
 * Ensure message queue is nearly empty on return.
 * Could be a race to empty message queue.
 */

bool SyncAgent::dispatchMsgUntil(
		DispatchFuncPtr dispatchQueuedMsg, // function to dispatch a message, knows desired msg type
		OSTime (*timeoutFunc)())	// function returning remaining duration of slot
{
	bool didReceiveDesiredMsg = false;
	bool didTimeout = false;

	/*
	 * There is a race here: caller just called receiveStatic().
	 * It takes ~0.1mSec per message (at 1Mb per second and 100bit messages)
	 * so we are allowed about 10k instructions (at 16Mhz, one instruction per Hz.)
	 * A receive must not complete before these assertions and the sleep,
	 * otherwise we will receive a message but sleep until timeout.
	 */
	assert(radio->isEnabledInterruptForMsgReceived());	// will interrupt
	// we beat the radio race, i.e. msg not already received
	assert(!radio->isDisabledState());	// is receiving

	//assert(sleeper.reasonForWakeIsCleared());	// This also checks we haven't received yet
	// TODO currently, this is being cleared in sleepUntil but that suffers from races

	while (true) {
		sleeper.sleepUntilEventWithTimeout(timeoutFunc());
		// wakened by msg or timeout

		switch (sleeper.getReasonForWake()) {
		case MsgReceived:
			didReceiveDesiredMsg = dispatchMsg(dispatchQueuedMsg);
			break;	// switch

		case TimerExpired:
			radio->stopReceive();
			// assert msg queue empty, except for race between timeout and receiver
			// Slot done.
			didTimeout = true;
			break;	// switch

		case None:
		default:
			// Unexpected reasonForWake or no IRQ handler set the reason
			assert(false);
			// Continue in loop and sleep again?
		}
		if (didReceiveDesiredMsg || didTimeout) break;	// while(true)
	}

	assert(radio->isDisabledState());  // not receiving
	// radio is on or off
	// ensure message queue nearly empty
	// ensure timeout or didReceiveDesiredMsg
	return didReceiveDesiredMsg;
}



bool SyncAgent::dispatchMsg(DispatchFuncPtr dispatchFuncOnType) {
	// FUTURE while any received messages queued

	bool didReceiveDesiredMsg = false;

	// Nested checks: physical layer CRC, then transport layer MessageType
	if (radio->isPacketCRCValid()) {
		Message* msg = serializer.unserialize();
		if (msg != nullptr) {
			// assert msg->type valid

			//ledLogger2.toggleLED(3);	// debug: LED 3 valid received

			didReceiveDesiredMsg = dispatchFuncOnType(msg);
			if (didReceiveDesiredMsg) {
				// Ultra low power sleep remainder of duration (radio power off)
				assert(radio->isDisabledState());
				radio->powerOff();
				// continuation is sleep
				// assert since radio power off, reason for wake can only be timeout and will then exit loop
			}
			else {
				/*
				 * Dispatched message was not of desired type (but we could have done work, or other state changes.)
				 * restart receive, remain in loop, sleep until next message
				 */
				radio->receiveStatic();
				// continuation is sleep
			}
			// assert msg queue is empty (since we received and didn't restart receiver)
		}
		else {
			// MessageType garbled
			//ledLogger2.toggleLED(4);	// debug: LED 4 invalid MessageType received
			// continuation is sleep
		}

		// All msg types freed
		freeReceivedMsg((void*) msg);
	}
	else {
		// ignore CRC invalid packet

		//ledLogger2.toggleLED(4);	// debug: LED 4 invalid CRC received
		// continuation is sleep
	}
	return didReceiveDesiredMsg;
}


