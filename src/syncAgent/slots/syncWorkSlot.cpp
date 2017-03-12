


#include <cassert>

#include "../globals.h"
#include "syncWorkSlot.h"
#include "syncSlotSchedule.h"
#include "../messageHandler/syncSlotMsgHandler.h"




namespace {

SyncSlotSchedule slotSchedule;
SyncSlotMessageHandler msgHandler;

} // namespace



bool SyncWorkSlot::doListenHalfSyncWorkSlot(OSTime (*timeoutFunc)()) {
	network.startReceiving();
	bool result = syncSleeper.sleepUntilMsgAcceptedOrTimeout(
			msgHandler.dispatchMsgReceived,
			timeoutFunc
			);

	// assert radio is on or off
	return result;
}


/*
 * Transmit WorkSync in middle.
 * App has queued work to be sent.
 */
void SyncWorkSlot::doSendingWorkSyncWorkSlot(){
	// not assert self is Master

	(void) doListenHalfSyncWorkSlot(slotSchedule.deltaToThisSyncSlotMiddleSubslot);
	assert(radio->isDisabledState());
	assert(radio->isPowerOn());

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
	// TODO, I might have heard Work, in that case I don't need to send it again.

	syncSender.sendWorkSync();

	/*
	 * Keep listening for other better Masters or WorkSync.
	 *
	 * Even if I heard a sync, I also sent one (to convey work.)
	 * I could still hear my Master, or other work.
	 *
	 * Result doesn't matter, slot is over and we proceed regardless whether we heard sync keeping msg.
	 */
	(void) doListenHalfSyncWorkSlot(slotSchedule.deltaToThisSyncSlotEnd);

	// assert radio on or off
}




#ifdef NOTUSED
// Sleep with radio off for remainder of sync slot
void SyncWorkSlot::doIdleSlotRemainder() {
	assert(!radio->isPowerOn());
	syncSleeper.sleepUntilTimeout(clique.schedule.deltaToThisSyncSlotEnd);
}
#endif

/*
 * listen for sync the whole period.
 */
void SyncWorkSlot::doSlaveSyncWorkSlot() {
	network.startReceiving();
	// This assertion is time sensitive, can't stay in production code
	assert(!radio->isDisabledState()); // listening for other's sync

	// Log delay from sync point to actual start listening.
	// logInt(clique.schedule.deltaPastSyncPointToNow()); log("<delta SP to sync listen.\n");

	(void) syncSleeper.sleepUntilMsgAcceptedOrTimeout(
			msgHandler.dispatchMsgReceived,
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

	bool heardSyncKeepingSync;

	heardSyncKeepingSync = doListenHalfSyncWorkSlot(slotSchedule.deltaToThisSyncSlotMiddleSubslot);
	assert(radio->isDisabledState());

	/*
	 * Might have heard:
	 * - a sync from a worse clique,
	 * - OR a synch-keeping msg
	 * Regardless, continue to listen, mainly for work.
	 */
	if (! heardSyncKeepingSync) {
		// Self is Master, send sync if didn't hear WorkSync or MergeSync
		syncSender.sendMasterSync();
	}

	// Keep listening for other better Masters and work.
	// Result doesn't matter, slot is over and we proceed whether we heard sync keeping sync or not.
	(void) doListenHalfSyncWorkSlot(slotSchedule.deltaToThisSyncSlotEnd);

	// assert radio on or off
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

	network.preamble();

	network.prepareToTransmitOrReceive();


	// Call shouldTransmitSync every time, since it needs calls sideeffect reset itself
	bool needXmitSync = syncBehaviour.shouldTransmitSync();

	/*
	 * Work is higher priority than ordinary sync.
	 * Work must be rare, lest it flood network and destroy sync
	 * (colliding too often with MergeSync or MasterSync.)
	 */
	if (workOutMailbox->isMail() ) {
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

	// Radio is on or off.  If on, we timeout'd while receiving
	network.stopReceiving();
	// Radio on or off
	// Turn radio off, workSlot may not need it on
	network.shutdown();

	// FUTURE we could do this elsewhere, e.g. start of sync slot so this doesn't delay the start of work slot
	if (!clique.isSelfMaster())
		clique.checkMasterDroppedOut();

	network.postlude();

	assert(!radio->isPowerOn());	// ensure
}

