
/*
 * THE sync slot of my schedule.
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
 * Contention (two syncs in same interval of a sync slot):
 * - situation c:
 *   two cliques may unwittingly be in sync (two MASTERS xmitting in same interval)
 * - situation b:
 *   merging: a Merger of a better clique may be xmitting sync FROM its sleeping slot to merge this clique into an offset syncSlot
 */

#include <cassert>

#include "../globals.h"
#include "syncSlot.h"




// Private


bool SyncSlot::doMasterListenHalfSyncSlot(OSTime (*timeoutFunc)()) {
	startReceiving();
	bool result = syncSleeper.sleepUntilMsgAcceptedOrTimeout(
					this, //dispatchMsgReceived,
					timeoutFunc
					);

	// assert radio is on or off
	return result;
}

// Sleep with radio off for remainder of sync slot
void SyncSlot::doIdleSlotRemainder() {
	assert(!radio->isPowerOn());
	syncSleeper.sleepUntilTimeout(clique.schedule.deltaToThisSyncSlotEnd);
}


void SyncSlot::doSlaveSyncSlot() {
	// listen for sync the whole period
	startReceiving();
	// This assertion is time sensitive, can't stay in production code
	assert(!radio->isDisabledState()); // listening for other's sync
	// TODO not using result?
	(void) syncSleeper.sleepUntilMsgAcceptedOrTimeout(
				this,
				clique.schedule.deltaToThisSyncSlotEnd);
	// TODO Low priority, obsolete. If we heard sync-keeping msg, now is not the end of the slot.  Idle?
}

/*
 * Transmit any sync in middle of slot.
 * Some literature refers to "guards" around the sync.
 * Syncing in middle has higher probability of being heard by drifted/skewed others.
 *
 * !!! The offset must be half the slot length, back to start of SyncPeriod
 */
void SyncSlot::doMasterSyncSlot() {

	bool heardSyncKeepingSync = doMasterListenHalfSyncSlot(clique.schedule.deltaToThisSyncSlotMiddleSubslot);
	assert(radio->isDisabledState());

	if (heardSyncKeepingSync) {
		// I was Master, but just relinquished it.
		// I heard sync so powerOff'd radio.
		assert(!clique.isSelfMaster());
		assert(!radio->isPowerOn());
		assert(radio->isDisabledState());	// not receiving
		doIdleSlotRemainder();
	}
	else {
		// Might have heard a sync from a worse clique
		syncSender.sendMasterSync();
		// Keep listening for other better Masters.
		// Result doesn't matter, slot is over and we proceed whether we heard sync keeping sync or not.
		(void) doMasterListenHalfSyncSlot(clique.schedule.deltaToThisSyncSlotEnd);

		// assert radio on or off
	}
}




/*
 * Sync handling is agnostic of role.isMaster or role.isSlave
 *
 * MasterSync and MergeSync handled the same.
 *
 * FUTURE discard multiple sync messages if they are queued
 */

bool SyncSlot::doMasterSyncMsg(SyncMessage* msg) { return syncBehaviour.doSyncMsg(msg); }

bool SyncSlot::doMergeSyncMsg(SyncMessage* msg) { return syncBehaviour.doSyncMsg(msg); }

bool SyncSlot::doAbandonMastershipMsg(SyncMessage* msg){
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


bool SyncSlot::doWorkMsg(SyncMessage* msg){
	// Received in wrong slot, from out-of-sync clique

	/*
	 * Design decision:
	 * Work is done even out of sync with others.
	 * Alternatively, don't do work that is out of sync.  Change this to ignore the msg if from inferior clique.
	 */
	syncAgent.relayWorkToApp(msg->getWorkPayload());

	/*
	 * Design: (see "duality" in WorkSlot)
	 * My SyncSlot aligns with other's WorkSlot => other's SyncSlot aligns with my last SleepingSlot.
	 * So eventually I would fish in my lastSleepingSlot and find other clique.
	 * But merger comes sooner if we don't waste info we just found.
	 *
	 * I might need to schedule a fish for it in my last Sleeping slot.
	 */
	if (clique.isOtherCliqueBetter(msg->masterID)) {
		/*
		 * Self is inferior.
		 * Join other clique.
		 * Other members of my clique should (and with high probably, will) do the same.
		 *
		 * Don't become a Merger and merge my old clique into superior clique,
		 * because that requires sending MergeSync at a displaced time in my new Schedule's SyncSlot.
		 * Any such merging would also contend with superior other's work slot.
		 */
		syncAgent.mangleWorkMsg(msg);
		clique.updateBySyncMessage(msg);
	}
	else {
		/*
		 * Self is superior.
		 * Other clique will continue to send work in my SyncSlot.
		 * But other should hear my master's MasterSync in its work slot, and join my clique.
		 */
		// FUTURE schedule to fish in my last sleeping slot, so I can become a merger.
		// But many of my cohort may also do this.
		// Only the master should do this.
	}
	return false;	// keep looking
}




/*
 * Transmit any sync in middle of slot.
 * Some literature refers to "guards" around the sync.
 * Syncing in middle has higher probability of being heard by drifted/skewed others.
 *
 * !!! The offset must be half the slot length, back to start of SyncPeriod
 */

void SyncSlot::perform() {
	prepareRadioToTransmitOrReceive();
	if (syncBehaviour.shouldTransmitSync())
		doMasterSyncSlot();
	else
		// isSlave or (isMaster and not xmitting (coin flip))
		doSlaveSyncSlot();

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



