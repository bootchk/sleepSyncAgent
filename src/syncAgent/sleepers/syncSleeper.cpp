
#include <cassert>

#include "syncSleeper.h"

// nRF5x lib
#include <clock/sleeper.h>	// pure class Sleeper
#include <exceptions/powerAssertions.h>
#include <exceptions/resetAssertions.h>

#include "oversleepMonitor.h"
#include "../../augment/timeMath.h"
#include "../logger.h"
#include "../message/serializer.h"
#include "../modules/schedule.h"

#include "../scheduleParameters.h"


namespace {

// debugging
unsigned int countSleeps = 0;	// inner calls to Sleeper, could be a few but not more than tens
ReasonForWake priorReasonForWake;	// reason for previous wake (before we cleared it, slept, and browned out.)

/*
 * Call sleeper after asserting certain peripherals are off.
 * All calls to Sleeper::sleepUntilEventWithTimeout() funnel through these methods.
 *
 * A Sleeper may wake for unexpected reasons.
 */

/*
 * radio ensemble (HFXO and DCDC) OFF
 * RTC and LXFO ON
 */
void sleepWithOnlyTimerPowerUntilTimeout(DeltaTime timeout) {
	assertUltraLowPower();
	assertNoResetsOccurred();
	Sleeper::sleepUntilEventWithTimeout(timeout);
}

/*
 * radio and its owned peripherals (hfClock and DCDC power supply) can be on.
 * RTC and LXFO are on.
 */
void sleepWithRadioAndTimerPowerUntilTimeout(DeltaTime timeout) {
	assertRadioPower();
	Sleeper::sleepUntilEventWithTimeout(timeout);
}



/*
 * Filter invalid messages.
 * Return result of dispatching valid messages.
 */
HandlingResult dispatchFilteredMsg( MessageHandler msgHandler) { // Slot has handlers per message type
	// FUTURE while any received messages queued

	HandlingResult handlingResult = HandlingResult::KeepListening;

	// Nested checks: physical layer CRC, then transport layer MessageType
	if (Ensemble::isPacketCRCValid()) {
		SyncMessage* msg = Serializer::unserialize();
		if (msg != nullptr) {
			// assert msg->type valid

			handlingResult = msgHandler(msg);
			if (handlingResult!=HandlingResult::KeepListening) {
				// Remainder of duration radio not used (low power) but HFXO is still on.
				// TODO this assertion should be at beginning of routine
				assert(!Ensemble::isRadioInUse());

				// continuation is sleep
				// assert since radio not in use, next reason for wake can only be timeout and will then exit loop
			}
			else {
				/*
				 * Dispatched message was not of desired type (but we could have done work, or other state changes.)
				 * restart receive, remain in loop, sleep until next message
				 */
				Ensemble::startReceiving();
				// continuation is sleep with radio on
			}
			// assert msg queue is empty (since we received and didn't restart receiver)
		}
		else {
			// Ignore garbled type or offset
			log(Logger::Garbled);
			// continuation is wait for another message
			Ensemble::startReceiving();
		}

		// No memory managment for messages
	}
	else {
		/* Ignore CRC invalid packet except to log it.
		 *
		 * Note CRCSTATUS register remains showing invalid until another message is received.
		 */
		log(Logger::CRC);
		// continuation is wait for another message
		Ensemble::startReceiving();

	}

	/*
	 * result "keep listening" implies we restarted the radio
	 * handlingResult==KeepListening=>Ensemble::isRadioInUse
	 * Logical implication P=>Q is equivalent to not P OR Q
	 * handlingResult!=KeepListening OR ensemble::isRadioInUse
	 * TODO add those assertions, for now, there is a downstream assertion
	 */
	return handlingResult;
}



HandlingResult determineHandlingResult(MessageHandler msgHandler) {
	/*
	 * We were wakened.
	 * Switch on reasonForWake
	 */
	HandlingResult handlingResult = HandlingResult::KeepListening;

	switch (Sleeper::getReasonForWake()) {

	// TODO, its a packet not a msg, could be invalid
	case ReasonForWake::MsgReceived:
		// Record TOA as soon as possible
		SyncMessage::timeOfArrival = LongClock::nowTime();

		// if timer semantics are: restartable, cancel timer here
		handlingResult = dispatchFilteredMsg(msgHandler);
		// Handler may ignore packet and startReceiving again

		break;	// switch

	case ReasonForWake::SleepTimerExpired:
		// Timeout could be interrupting a receive.
		// Better to handle message and delay next slot: fewer missed syncs.

		Ensemble::stopReceiving();
		// assert msg queue empty, except for race between timeout and receiver
		// Slot done.
		handlingResult = HandlingResult::TimedOut;
		break;	// switch
		// FUTURE try handle receive in progress, see code fragment at eof

	case ReasonForWake::CounterOverflowOrOtherTimerExpired:
		/*
		 * Expected events not relevant to this sleep.
		 * KeepListening
		 */
		// assert Ensemble::isRadioInUse()
		break;

	case ReasonForWake::Unknown:
		/*
		 * Unexpected: No IRQ handler set reason reasonForWake.
		 * KeepListening
		 */
		Logger::logUnexpectedWakeWhileListening();
		// assert Ensemble::isRadioInUse()
		break;

	case ReasonForWake::BrownoutWarning:
		/*
		 * This is just warning.  BOR reset might not happen.
		 * Already logged by brownout handler.
		 * KeepListening, but might BOR
		 */
		// TODO we should stop listening more gracefully
		break;

	case ReasonForWake::Cleared:
	case ReasonForWake::HFClockStarted:
	case ReasonForWake::LFClockStarted:
		// Impossible, Sleeper will not return without reason
		// Impossible, LFClock and HFClock started earlier
		assert(false);
	}

	return handlingResult;
}



/*
 * Calculate timeout scalar:
 * - using a TimeoutFunc
 * - adjusting for overhead
 */
DeltaTime calculateTimeout(TimeoutFunc timeoutFunc) {
	return TimeMath::clampedSubtraction(timeoutFunc(), ScheduleParameters::CodesSleepOverhead);
}


}	// namespace





// OLD void SyncSleeper::clearReasonForWake() { Sleeper::clearReasonForWake(); }


/*
 * Sleep until timeout, ensuring that timeout time has elapsed:
 * - ignoring any unexpected events
 *
 * Receiver is off => no messages should be received.
 */
void SyncSleeper::sleepUntilTimeout(TimeoutFunc timeoutFunc) {

	OverSleepMonitor::markStartSleep(timeoutFunc);
	countSleeps = 0;

	do {
		// debugging
		countSleeps++;
		priorReasonForWake = Sleeper::getReasonForWake();

		// Calculate remaining timeout on each loop iteration.  Must be monotonic.
		OSTime timeout = calculateTimeout(timeoutFunc);

		sleepWithOnlyTimerPowerUntilTimeout(timeout);
		// an event, or we did not sleep at all (timeout small)

		/*
		 * If a higher priority event occurs between the time that sleeper set reasonForWake to SleepTimerExpired
		 * (because the timeout was small)
		 * and the following check, then we will loop another time.
		 * Eventually, the call to the sleeper with small timout will set reasonForWake to SleepTimerExpired.
		 */
		// TODO but sleep... is clearing reasonForWake
	}
	while (! Sleeper::isWakeForTimerExpired());
	/*
	 * timeoutFunc is monotonic and will eventually return 0
	 * and Sleeper::sleepUntilTimeout will return without sleeping and with reasonForWake==Timeout
	 */

	(void) OverSleepMonitor::checkOverslept();
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
/*
 * The design depends on Timer semantics: can a Timer be restarted?
 * Here, we assume not, and always that Timer was canceled.
 */
HandlingResult SyncSleeper::sleepUntilMsgAcceptedOrTimeout (
		MessageHandler msgHandler,
		TimeoutFunc timeoutFunc)	// function returning remaining duration of slot
{
	HandlingResult handlingResult = HandlingResult::KeepListening;

	/*
	 * There is a race here: caller just called receiveStatic().
	 * It takes ~0.1mSec per message (at 1Mb per second and 100bit messages)
	 * so we are allowed about 10k instructions (at 16Mhz, one instruction per Hz.)
	 * A receive must not complete before these assertions and the sleep,
	 * otherwise we will receive a message but sleep until timeout.
	 */
	//assert(radio->isEnabledInterruptForMsgReceived());	// will interrupt
	// we beat the radio race, i.e. msg not already received
	assert(Ensemble::isRadioInUse());

	//assert(Sleeper::reasonForWakeIsCleared());	// This also checks we haven't received yet
	// FUTURE currently, this is being cleared in sleepUntil but that suffers from races

	OverSleepMonitor::markStartSleep(timeoutFunc);
	countSleeps = 0;

	// loop invariant ??

	do {
		// We only loop to listen: assert radio on
		assert(Ensemble::isRadioInUse());
		//  TODO && (! Timer::isStarted()));

		// debugging
		countSleeps++;
		priorReasonForWake = Sleeper::getReasonForWake();

		OSTime timeout = calculateTimeout(timeoutFunc);

		sleepWithRadioAndTimerPowerUntilTimeout(timeout);
		/*
		 * wakened by msg or timeout or unexpected event or did not sleep at all (timeout small)
		 * When msg received (even if CRC bad or msg garbled) the receiver is off.
		 */

		Sleeper::cancelTimeout();

		handlingResult = determineHandlingResult(msgHandler);

		// If Timer semantics are restartable: timer might be canceled, but sleepUntilEventWithTimeout will restart it

#ifndef NDEBUG
		// Check overslept whenever we are wakened
		if (OverSleepMonitor::checkOverslept()) {
			Ensemble::stopReceiving();
			// handlingResult is invalid
			break;
		}
#endif
	}
	/*
	 * until (timed out OR received desired message)
	 * equiv to while (!(timed out OR received desired message))
	 */
	while ( !(
			(handlingResult == HandlingResult::TimedOut)
			|| (handlingResult != HandlingResult::KeepListening)
			));
	/*
	 * assert the timeoutFunc() will eventually return zero and not sleep with reason==TimerExpired
	 */

	/*
	 * Robustness:ensure not sleep too long with radio powered.
	 * Probably a fixable bug.  Possibly hardware flaws that can't be fixed.
	 */
	(void) OverSleepMonitor::checkOverslept();

	assert(!Ensemble::isRadioInUse());
	// assert time is canceled
	// not assert Ensemble::isLowPower(), HFXO is still on
	// ensure message queue nearly empty
	// ensure timeout or didReceiveDesiredMsg
	return handlingResult;
}



MsgReceivedCallback SyncSleeper::getMsgReceivedCallback() {
	// Return callback of the owned/wrapped Sleeper::
	return Sleeper::msgReceivedCallback;
}

unsigned int SyncSleeper::getCountSleeps() { return countSleeps; }
unsigned int SyncSleeper::getPriorReasonForWake() { return (unsigned int) priorReasonForWake; }




/*
 * Similar above, but overloaded: different parameter type
 * and different implementation.
 */
void SyncSleeper::sleepUntilTimeout(DeltaTime timeout)
{
	LongTime endingTime = LongClock::nowTime() + timeout;
	DeltaTime remainingTimeout = timeout;

	while (true) {

			assert(remainingTimeout < ScheduleParameters::MaxSaneTimeout);

			/*
			 * !!! Just sleep, with no assertions on power.
			 */
			Sleeper::sleepUntilEventWithTimeout(remainingTimeout);
			// sleepWithOnlyTimerPowerUntilTimeout(timeout);

			if (Sleeper::isWakeForTimerExpired()) {
				break;	// while true, assert time timeout has elapsed.
			}
			else {
				// reasonForWake is not TimerExpired
				remainingTimeout = TimeMath::clampedTimeDifferenceFromNow(endingTime);
				// continue next loop iteration
			}
			/*
			 * waking events spend time, is monotonic and will eventually return 0
			 * and Sleeper::sleepUntilTimeout will return without sleeping and with reasonForWake==Timeout
			 */
		}
		// assert timeout amount of time has elapsed
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

