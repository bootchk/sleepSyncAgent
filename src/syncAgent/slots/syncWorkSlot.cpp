
/*
 * Combined work/sync slot.
 * WorkSync message carries sync.
 *
 * Where sync period has one sync slot and no separate work slot.
 *
 * Each Clique is on a Schedule that includes a sync slot.
 * See general notes in Schedule.
 *
 * a) A clique's Master xmits sync FROM its sync slot.
 *
 * b) A Merger xmits sync FROM a sleeping slot INTO another clique's sync slot.
 *
 * c) One clique's sleeping slot may coincidentally be near in time to another clique's sleeping slot, or drift into each other.
 *
 * d) A clique Master or Slave may xmit work from this slot, carrying sync info
 *
 * Contention (two syncs in same interval of a sync slot):
 * - situation c:
 *   two cliques may unwittingly be in sync (two MASTERS xmitting in same interval)
 * - situation b:
 *   merging: a Merger of a better clique may be xmitting sync FROM its sleeping slot to merge this clique into an offset syncSlot
 */

#include <cassert>

#include "../globals.h"
#include "syncWorkSlot.h"

#include "../logMessage.h"




// Private

/*
 * Transmit WorkSync in middle.
 */
void SyncWorkSlot::doSendingWorkSyncWorkSlot(){
	// not assert self is Master

	(void) doListenHalfSyncWorkSlot(clique.schedule.deltaToThisSyncSlotMiddleSubslot);
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

	syncSender.sendWorkSync();

	/*
	 * Keep listening for other better Masters or WorkSync.
	 *
	 * Even if I heard a sync, I also sent one (whose main purpose was to convey work.)
	 * I could still hear my Master, or other work.
	 *
	 * Result doesn't matter, slot is over and we proceed regardless whether we heard sync keeping msg.
	 */
	(void) doListenHalfSyncWorkSlot(clique.schedule.deltaToThisSyncSlotEnd);

	// assert radio on or off
}



bool SyncWorkSlot::doListenHalfSyncWorkSlot(OSTime (*timeoutFunc)()) {
	startReceiving();
	bool result = syncSleeper.sleepUntilMsgAcceptedOrTimeout(
					this,
					timeoutFunc
					);

	// assert radio is on or off
	return result;
}

#ifdef NOTUSED
// Sleep with radio off for remainder of sync slot
void SyncWorkSlot::doIdleSlotRemainder() {
	assert(!radio->isPowerOn());
	syncSleeper.sleepUntilTimeout(clique.schedule.deltaToThisSyncSlotEnd);
}
#endif

void SyncWorkSlot::doSlaveSyncWorkSlot() {
	// listen for sync the whole period
	startReceiving();
	// This assertion is time sensitive, can't stay in production code
	assert(!radio->isDisabledState()); // listening for other's sync

	(void) syncSleeper.sleepUntilMsgAcceptedOrTimeout(
				this, //dispatchMsgReceived,
				clique.schedule.deltaToThisSyncSlotEnd);
	/*
	 * Not using result:  all message handlers return false i.e. keep looking.
	 * Assert we timed out and now is end of slot.
	 */
}


/*
 * Transmit any sync in middle of slot.
 * Some literature refers to "guards" around the sync.
 * Syncing in middle has higher probability of being heard by drifted/skewed others.
 *
 * !!! The offset must be half the slot length, back to start of SyncPeriod
 */


void SyncWorkSlot::doMasterSyncWorkSlot() {

	bool heardSyncKeepingSync;

	heardSyncKeepingSync = doListenHalfSyncWorkSlot(clique.schedule.deltaToThisSyncSlotMiddleSubslot);
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
	(void) doListenHalfSyncWorkSlot(clique.schedule.deltaToThisSyncSlotEnd);

	// assert radio on or off
}




/*
 * Sync handling is agnostic of role.isMaster or role.isSlave
 *
 * MasterSync and MergeSync handled the same.
 *
 * FUTURE discard multiple sync messages if they are queued
 */

bool SyncWorkSlot::doMasterSyncMsg(SyncMessage* msg) {
	(void) syncBehaviour.doSyncMsg(msg);
	return false;	// keep looking
}

bool SyncWorkSlot::doMergeSyncMsg(SyncMessage* msg) {
	(void) syncBehaviour.doSyncMsg(msg);
	return false;
}

bool SyncWorkSlot::doAbandonMastershipMsg(SyncMessage* msg){
	/*
	 * My clique is still in sync, but master is dropout.
	 *
	 * Naive design: all units that hear master abandon assume mastership.
	 * FUTURE: keep historyOfMasters, and better slaves assume mastership.
	 */
	(void) msg;  // FUTURE use msg to record history

	clique.setSelfMastership();
	assert(clique.isSelfMaster());
	return false;	// keep listening
}


bool SyncWorkSlot::doWorkMsg(SyncMessage* msg){

	/*
	 * Doesn't matter which clique it came from, relay the work.
	 * I.E. handle the work aspect of message.
	 */
	syncAgent.relayWorkToApp(msg->workPayload());

	/*
	 *  Handle the sync aspect of message.
	 */
	syncBehaviour.doSyncMsg(msg);

	return false;	// keep looking
}




/*
 * Transmit any sync in middle of slot.
 * Some literature refers to "guards" around the sync.
 * Syncing in middle has higher probability of being heard by drifted/skewed others.
 *
 * !!! The offset must be half the slot length, back to start of SyncPeriod
 */

void SyncWorkSlot::perform() {
	prepareRadioToTransmitOrReceive();

	/*
	 * Work is higher priority than ordinary sync.
	 * Work must be rare, lest it flood network and destroy sync
	 * (colliding too often with MergeSync or MasterSync.)
	 */
	if (workOutMailbox->isMail() ) {
		doSendingWorkSyncWorkSlot();
	}
	else {
		// No work to send, maintain sync if master
		if (syncBehaviour.shouldTransmitSync())
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
	stopReceiving();
	// Radio on or off
	// Turn radio off, workSlot may not need it on
	shutdownRadio();

	// FUTURE we could do this elsewhere, e.g. start of sync slot so this doesn't delay the start of work slot
	if (!clique.isSelfMaster())
		clique.checkMasterDroppedOut();

	assert(!radio->isPowerOn());	// ensure
}



