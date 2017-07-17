
#include <cassert>

#include "../globals.h"
#include "../modules/syncSender.h"
#include "syncWorkSlot.h"
#include "syncSlotSchedule.h"
#include "../messageHandler/messageHandler.h"


namespace {

/*
 * Did we already hear a Sync message for our clique?
 *
 * Might have heard:
 * - a sync from a worse clique (result KeepListening)
 * - OR a synch-keeping msg (Master, Merge, or WorkSync)
 * - garbled messages or non-sync keeping messages (result KeepListening)
 * - AbandonMastership (from some confused clique that overlaps ours?)
 *
 * !!! A WorkSync might be from a slave, but it briefly keeps sync for our clique,
 * and we omit one MasterSync but don't relinquish Mastership.
 */
bool didFirstHalfHearSync(HandlingResult firstHalfResult){
	bool result;
	switch (firstHalfResult) {
	case HandlingResult::TimedOut:
	case HandlingResult::KeepListening:
		result = false;
		break;
	case HandlingResult::StopListeningHeardMasterSync:
	case HandlingResult::StopListeningHeardMergeSync:
	case HandlingResult::StopListeningHeardWorkSync:
		result = true;
	}
	return result;
}

}


HandlingResult SyncWorkSlot::doListenHalfSyncWorkSlot(TimeoutFunc timeoutFunc) {

	/*
	 * This IF is mainly for debugging?
	 * Starting the HFXO (previously) or anything else that preceded this call
	 * is not expected to exhaust power.
	 */
	if (SyncPowerManager::isPowerForRadio()) {
		Ensemble::startReceiving();
	}
	else {
		Logger::logNoPowerForHalfSyncSlot();
		// Note HFXO is still running
		// Continue to sleep for half a slot: we may yet xmit sync, and/or listen for second half.
	}

	HandlingResult result = SyncSleeper::sleepUntilMsgAcceptedOrTimeout(
			SyncSlotMessageHandler::handle,
			timeoutFunc
			);

	/*
	 *  not assert Ensemble::isLowPower():
	 *  The radio might be in use (receiving.)
	 *  The continuation might be send sync and listen another half.
	 *  That continuation needs the radio.
	 */
	return result;
}


/*
 * Transmit WorkSync in middle.
 * App has queued work to be sent.
 */
void SyncWorkSlot::doSendingWorkSyncWorkSlot(){
	// not assert self is Master

	Phase::set(PhaseEnum::SyncListenFirstHalf);
	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot);
	assert(!Ensemble::isRadioInUse());

	/*
	 * Even if I heard sync, need send Work.
	 * assert self is Master or Slave.
	 * Might have heard sync from:
	 * - worse clique
	 * - master
	 * - better clique (and self if Master might have relinquished mastership.)
	 *
	 * Even if my clique changed, need to send workSync to it.
	 */

	/*
	 * Design choice: all work is equivalent: don't send if others sent.
	 *
	 * Also, this knows that we bounce out work back to app.
	 */
	// Fetch work from app
	WorkPayload work = workOutMailbox->fetch();
	if (! workManager.isHeardWork() ) {
		Phase::set(PhaseEnum::SyncXmitWorkSync);
		SyncSender::sendWorkSync(work);
		/*
		 * App sent this work.  App also queues it in to itself if app wants to work as well as tell others to work.
		 */
	}
	else {
		// assert heard work is relayed to app via callback
		/*
		 * FUTURE: work is distinct and mailbox holds many work
		 * send distinct work
		 * and queue another one from app back to app
		 */
	}


	/*
	 * Keep listening for other better Masters or WorkSync.
	 *
	 * Even if I heard a sync, I also sent one (to convey work.)
	 * I could still hear my Master, or other work.
	 *
	 * Result doesn't matter, slot is over and we proceed regardless whether we heard sync keeping msg.
	 */
	Phase::set(PhaseEnum::SyncListenSecondHalf);
	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotEnd);

	// not assert Ensemble::isLowPower()
}





// Sleep with network shutdown for remainder of sync slot
void SyncWorkSlot::sleepSlotRemainder() {
	//assert(!netork->isInUse());
	SyncSleeper::sleepUntilTimeout(SyncSlotSchedule::deltaToThisSyncSlotEnd);
}


/*
 * listen for sync the whole period.
 */
void SyncWorkSlot::doSlaveSyncWorkSlot() {


	//LogMessage::logListenFullSlot();
	Ensemble::startReceiving();

	// Assert listening for other's sync.
	// Cannot assert isRadioInUse() since receive might have succeeded already (when using debugger)

	// Log delay from sync point to actual start listening.
	// logInt(clique.schedule.deltaPastSyncPointToNow()); log("<delta SP to sync listen.\n");

	Phase::set(PhaseEnum::SyncSlaveListen);
	(void) SyncSleeper::sleepUntilMsgAcceptedOrTimeout(
			SyncSlotMessageHandler::handle,
			SyncSlotSchedule::deltaToThisSyncSlotEnd);
	/*
	 * Not using result:  all message handlers return false i.e. keep looking.
	 * Assert we timed out and now is end of slot.
	 */
}



/*
 * Transmit any sync in middle of slot.
 */
void SyncWorkSlot::doMasterSyncWorkSlot() {

	HandlingResult handlingResult;

	Phase::set(PhaseEnum::SyncListenFirstHalf);
	handlingResult = doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot);
	assert(!Ensemble::isRadioInUse());

	/*
	 * Moment to xmit sync.  (We are Master.)
	 * But only if we didn't already (in first half) hear a message that keeps sync for our clique.
	 */
	if ( ! didFirstHalfHearSync(handlingResult)) {
		// Self is Master, send sync if didn't hear WorkSync or MergeSync
		Phase::set(PhaseEnum::SyncXmit);
		SyncSender::sendMasterSync();
	}

	/*
	 * Keep listening for other better Masters that might have fished us (MergeSync) and WorkSync.
	 * Result of this listening doesn't matter, slot is over and we proceed whether we heard sync keeping sync or not.
	 */
	Phase::set(PhaseEnum::SyncListenSecondHalf);
	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotEnd);

	// not assert Ensemble::isLowPower()
}

// XXX try doing part of the sync slot i.e. fail after the first half because power is exhausted.
// XXX if we heard a MergeSync in the first half, no point in listening for the second half. (minor optimization.)


/*
 * SyncSlot is first.
 * Assert already checked isPowerForSync(),
 */
void SyncWorkSlot::tryPerform() {

	/*
	 * Start network before deciding what kind of sync slot to perform.
	 */
	// Sleeps until ensemble ready. Deadtime in slot.
	// TIMING: > 360uSec, as much as 1.2mSec
	// Current:
	//LongTime startTime = clique.schedule.nowTime();
	Ensemble::startup();
	//LongTime endTime = clique.schedule.nowTime();

	// Starting network might have exhausted power.  Unlikely
	if (SyncPowerManager::isPowerForRadio()) {
		Phase::set(PhaseEnum::SyncChooseKind);
		perform();
	}
	else {
		Phase::set(PhaseEnum::SyncSkipSlot);
		Logger::logNoPowerToStartSyncSlot();

		// Busted SyncSlot, no listen, no send sync
		sleepSlotRemainder();

		// Continuation will be sleep to FishSlot

		// Maybe HFXO will start faster on second iterations?
	}
}



/*
 * Transmit any sync in middle of slot.
 * Some literature refers to "guards" around the sync.
 * Syncing in middle has higher probability of being heard by drifted/skewed others.
 *
 * !!! The offset must be half the slot length, back to start of SyncPeriod
 */

void SyncWorkSlot::perform() {
	// logInt(clique.schedule.deltaPastSyncPointToNow()); log("<delta SP to start slot.\n");

	// assert network is started

	// Call shouldTransmitSync every time, since it needs calls side effect reset itself
	bool needXmitSync = syncBehaviour.shouldTransmitSync();

	/*
	 * Work is higher priority than ordinary sync.
	 * Work must be rare, lest it flood airwaves and destroy sync
	 * (colliding too often with MergeSync or MasterSync.)
	 */
	if (workManager.isNeedSendWork()) {
		// This satisfies needXmitSync
		doSendingWorkSyncWorkSlot();
	}
	else {
		// No work to send, maintain sync if master
		if (needXmitSync)
			doMasterSyncWorkSlot();
		else
			// isSlave or (isMaster and not xmitting (coin flip))
			doSlaveSyncWorkSlot();
	}

	/*
	 * This may be late, when message receive thread delays this.
	 * Also, there could be a race to deliver message with this event.
	 * FUTURE check for those cases.
	 * Scheduling of subsequent events does not depend on timely this event.
	 */

	// Radio might be in use.  In use: we timeout'd while receiving
	Ensemble::stopReceiving();

	// FUTURE we could do this elsewhere, e.g. start of sync slot so this doesn't delay the start of work slot
	if (!clique.isSelfMaster())
		clique.checkMasterDroppedOut();

	Ensemble::shutdown();

	assert(Ensemble::isLowPower());
}

