


#include <cassert>

#include "../globals.h"
#include "syncWorkSlot.h"
#include "syncSlotSchedule.h"
#include "../messageHandler/messageHandler.h"




namespace {

SyncSlotSchedule slotSchedule;
SyncSlotMessageHandler msgHandler;

} // namespace



HandlingResult SyncWorkSlot::doListenHalfSyncWorkSlot(OSTime (*timeoutFunc)()) {

	/*
	 * This IF is mainly for debugging?
	 * Starting the HFXO (previously) or anything else that preceded this call
	 * is not expected to exhaust power.
	 */
	if (syncPowerManager->isPowerForRadio()) {
		LogMessage::logListenHalfSlot();
		network.startReceiving();
	}
	else {
		LogMessage::logNoPowerForHalfSyncSlot();
		// Note HFXO is still running
		// Continue to sleep for half a slot: we may yet xmit sync, and/or listen for second half.
	}

	HandlingResult result = syncSleeper.sleepUntilMsgAcceptedOrTimeout(
			&msgHandler,
			timeoutFunc
			);

	/*
	 *  not assert network.isLowPower():
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

	(void) doListenHalfSyncWorkSlot(slotSchedule.deltaToThisSyncSlotMiddleSubslot);
	assert(!network.isRadioInUse());

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
		syncSender.sendWorkSync(work);
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
	(void) doListenHalfSyncWorkSlot(slotSchedule.deltaToThisSyncSlotEnd);

	// not assert network.isLowPower()
}




#ifdef NOTUSED
// Sleep with radio off for remainder of sync slot
void SyncWorkSlot::doIdleSlotRemainder() {
	assert(!radio->isInUse());
	syncSleeper.sleepUntilTimeout(clique.schedule.deltaToThisSyncSlotEnd);
}
#endif

/*
 * listen for sync the whole period.
 */
void SyncWorkSlot::doSlaveSyncWorkSlot() {
	LogMessage::logListenFullSlot();
	network.startReceiving();

	// Assert listening for other's sync.
	// Cannot assert isRadioInUse() since receive might have succeeded already (when using debugger)

	// Log delay from sync point to actual start listening.
	// logInt(clique.schedule.deltaPastSyncPointToNow()); log("<delta SP to sync listen.\n");

	(void) syncSleeper.sleepUntilMsgAcceptedOrTimeout(
			&msgHandler,
			slotSchedule.deltaToThisSyncSlotEnd);
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

	handlingResult = doListenHalfSyncWorkSlot(slotSchedule.deltaToThisSyncSlotMiddleSubslot);
	assert(!network.isRadioInUse());

	/*
	 * Might have heard:
	 * - a sync from a worse clique,
	 * - OR a synch-keeping msg
	 * Regardless, continue to listen, mainly for work.
	 */
	if (handlingResult == HandlingResult::KeepListening) {
		// Self is Master, send sync if didn't hear WorkSync or MergeSync
		syncSender.sendMasterSync();
	}

	// Keep listening for other better Masters and work.
	// Result doesn't matter, slot is over and we proceed whether we heard sync keeping sync or not.
	(void) doListenHalfSyncWorkSlot(slotSchedule.deltaToThisSyncSlotEnd);

	// not assert network.isLowPower()
}

// TODO try doing part of the sync slot i.e. fail after the first half.


/*
 * Since SyncSlot is first and we already checked isPowerForSync(),
 * the check isPowerForRadio should always succeed.
 */
void SyncWorkSlot::tryPerform() {
	if (syncPowerManager->isPowerForRadio()) {
		perform();
	}
	else {
		LogMessage::logNoPowerToStartSyncSlot();
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

	// Sleep until network ready. Deadtime in slot.
	network.startup();

	// Call shouldTransmitSync every time, since it needs calls sideeffect reset itself
	bool needXmitSync = syncBehaviour.shouldTransmitSync();

	/*
	 * Work is higher priority than ordinary sync.
	 * Work must be rare, lest it flood network and destroy sync
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
	network.stopReceiving();

	// FUTURE we could do this elsewhere, e.g. start of sync slot so this doesn't delay the start of work slot
	if (!clique.isSelfMaster())
		clique.checkMasterDroppedOut();

	network.shutdown();

	assert(network.isLowPower());
}

