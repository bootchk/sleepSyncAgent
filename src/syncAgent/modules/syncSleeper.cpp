
#include <cassert>

#include <modules/powerAssertions.h>	// nRF5x lib

#include "../globals.h"
#include "syncSleeper.h"

#include "../scheduleParameters.h"
#include "../logMessage.h"


namespace {
Sleeper sleeper;
LongClockTimer* longClockTimer;	// for toa


/*
 * Call sleeper after asserting certain peripherals are off.
 * All calls to sleeper.sleepUntilEventWithTimeout() funnel through these methods.
 *
 * A Sleeper may wake for unexpected reasons.
 */


/*
 * radio and its owned peripherals (hfClock and DCDC power supply) OFF
 * RTC and LXFO ON
 */
void sleepWithOnlyTimerPowerUntilTimeout(DeltaTime timeout) {
	assertUltraLowPower();
	sleeper.sleepUntilEventWithTimeout(timeout);
}

/*
 * radio and its owned peripherals (hfClock and DCDC power supply) can be on.
 * RTC and LXFO are on.
 */
void sleepWithRadioAndTimerPowerUntilTimeout(DeltaTime timeout) {
	assertRadioPower();
	sleeper.sleepUntilEventWithTimeout(timeout);
}



/*
 * Filter invalid messages.
 * Return result of dispatching valid messages.
 */
HandlingResult dispatchFilteredMsg( MessageHandler* msgHandler) { // Slot has handlers per message type
	// FUTURE while any received messages queued

	HandlingResult didReceiveDesiredMsg = KeepListening;

	// Nested checks: physical layer CRC, then transport layer MessageType
	if (radio->isPacketCRCValid()) {
		SyncMessage* msg = serializer.unserialize();
		if (msg != nullptr) {
			// assert msg->type valid

			// TODO quiet conversion to bool here
			didReceiveDesiredMsg = msgHandler->handle(msg);
			if (didReceiveDesiredMsg) {
				// Remainder of duration radio not used (low power) but HFXO is still on.
				assert(!network.isRadioInUse());

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
			// Ignore garbled type or offset
			log(LogMessage::Garbled);
			// continuation is sleep
		}

		// No memory managment for messages
	}
	else {
		/* Ignore CRC invalid packet except to log it.
		 *
		 * Note CRCSTATUS register remains showing invalid until another message is received.
		 */
		log(LogMessage::CRC);
		// continuation is sleep
	}
	return didReceiveDesiredMsg;
}

}	// namespace


void SyncSleeper::init(
		OSTime maxSaneTimeout,
		LongClockTimer * aLCT)
{
	sleeper.init(maxSaneTimeout, aLCT);
	longClockTimer = aLCT;
}

void SyncSleeper::clearReasonForWake() { sleeper.clearReasonForWake(); }


/*
 * Sleep until timeout, ensuring that timeout time has elapsed:
 * - ignoring any unexpected events
 *
 * Receiver is off, so no messages can be received.
 */
void SyncSleeper::sleepUntilTimeout(OSTime (*timeoutFunc)()) {
	while (true) {
		// Calculate remaining timeout on each loop iteration
		OSTime timeout = timeoutFunc();

		assert(timeout < ScheduleParameters::MaxSaneTimeout);

		sleepWithOnlyTimerPowerUntilTimeout(timeout);
		// Expect wake by timeout, not by msg or other event
		if ( sleeper.getReasonForWake() == TimerExpired)
			// assert time specified by timeoutFunc has elapsed.
			break;	// while true
		else {
			// reasonForWake is not TimerExpired, e.g. an unexpected reason
			// continue next loop iteration
		}
		/*
		 * timeoutFunc is monotonic and will eventually return 0
		 * and sleeper.sleepUntilTimeout will return without sleeping and with reasonForWake==Timeout
		 */
	}
	// assert timeout amount of time has elapsed
}

/*
 * Similar above, but overloaded: different parameter type
 * and different implementation.
 */
void SyncSleeper::sleepUntilTimeout(DeltaTime timeout)
{
	LongTime endingTime = longClockTimer->nowTime() + timeout;
	DeltaTime remainingTimeout = timeout;

	while (true) {

			assert(remainingTimeout < ScheduleParameters::MaxSaneTimeout);

			sleepWithOnlyTimerPowerUntilTimeout(timeout);
			// Expect wake by timeout, not by msg or other event
			if ( sleeper.getReasonForWake() == TimerExpired)
				// assert time timeout has elapsed.
				break;	// while true
			else {
				// reasonForWake is not TimerExpired, e.g. an unexpected reason
				remainingTimeout = TimeMath::clampedTimeDifferenceFromNow(endingTime);
				// continue next loop iteration
			}
			/*
			 * waking events spend time, is monotonic and will eventually return 0
			 * and sleeper.sleepUntilTimeout will return without sleeping and with reasonForWake==Timeout
			 */
		}
		// assert timeout amount of time has elapsed
}

/*
 * Sleep until timeout with radio on.
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

HandlingResult SyncSleeper::sleepUntilMsgAcceptedOrTimeout(
		MessageHandler* msgHandler,
		OSTime (*timeoutFunc)())	// function returning remaining duration of slot
{
	HandlingResult didReceiveDesiredMsg = KeepListening;
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
	assert(network.isRadioInUse());

	//assert(sleeper.reasonForWakeIsCleared());	// This also checks we haven't received yet
	// FUTURE currently, this is being cleared in sleepUntil but that suffers from races

	while (true) {

		/*
		 * The design depends on Timer semantics: can a Timer be restarted?
		 * Here, we assume not, and always that Timer was canceled.
		 */
		sleepWithRadioAndTimerPowerUntilTimeout(timeoutFunc());
		// wakened by msg or timeout or unexpected event

		sleeper.cancelTimeout();

		switch (sleeper.getReasonForWake()) {
		case MsgReceived:
			// Record TOA as soon as possible
			clique.schedule.recordMsgArrivalTime();

			// if timer semantics are: restartable, cancel timer here
			didReceiveDesiredMsg = dispatchFilteredMsg(msgHandler);
			break;	// switch

		case TimerExpired:
			// Timeout could be interrupting a receive.
			// Better to handle message and delay next slot: fewer missed syncs.

			radio->stopReceive();
			// assert msg queue empty, except for race between timeout and receiver
			// Slot done.
			didTimeout = true;
			break;	// switch
			// FUTURE try handle receive in progress, see code fragment at eof

		case None:
		default:
			// Unexpected reasonForWake or no IRQ handler set the reason
			// FUTURE put this in some Handler to see?  But I already know what handlers are called.
			//#include "app_util_platform.h"
			// uint32_t ipsr = __get_IPSR();

			// Insert this code to know that this DOES happen
			// assert(false);

			// For now the solution is: continue in loop and sleep again.
			// assert the timeoutFunc() will eventually return zero and not sleep with reason==TimerExpired
			log("Unexpected wake, resume sleep.\n")
			;
		}
		// If Timer semantics are restartable: timer might be canceled, but sleepUntilEventWithTimeout will restart it

		if (didReceiveDesiredMsg || didTimeout) {
			/*
			 * assert radio off and timer cancelled.
			 */
			break;	// while(true)
		}
		// else continue while(true)

	}	// while(true)

	assert(!network.isRadioInUse());
	// not assert network.isLowPower()
	// ensure message queue nearly empty
	// ensure timeout or didReceiveDesiredMsg
	return didReceiveDesiredMsg;
}


msgReceivedCallback SyncSleeper::getMsgReceivedCallback() {
	// Return callback of the owned/wrapped sleeper.
	return sleeper.msgReceivedCallback;
}


#ifdef FUTURE
			//This experiment doesn't work see my post in DevZone
			if (radio->isReceiveInProgress()) {
				/*
				 * !!! Still interrupt enabled for Disabled i.e. receive complete.
				 * Alternative: disable interrupt and just spin here, but there would be a race.
				 *
				 * We can't just continue in the loop, since packet being received may be invalid.
				 *
				 * Let the interrupt come, but ignore it.
				 */
				log("Recv in progress\n");
				// continuation is: loop and sleep again

				radio->spinUntilReceiveComplete();
				radio->clearReceiveInProgress();
				didReceiveDesiredMsg = dispatchFilteredMsg(dispatchQueuedMsg);  // Often false??

				// We did timeout, i.e. allotted time for slot, etc. is over.
				didTimeout = true;
				// both didTimeout and didReceiveDesiredMsg could be true
			}
			else {
#endif

