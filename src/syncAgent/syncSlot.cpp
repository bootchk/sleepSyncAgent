
/*
 * SyncAgent methods used during THE sync slot of my schedule.
 *
 * General notes about slot implementations.
 * Slot is not a class.
 * All methods belong to SyncAgent, private.
 *
 * Each Clique is on a Schedule that includes a sync slot.
 * See general notes in Schedule.
 *
 * One clique's sleeping slot may be near the time one of another clique's sleeping slots.
 * A clique's Master xmits sync FROM its sync slot.
 * A Merger xmits sync FROM a sleeping slot INTO another clique's sync slot.
 * Contention (two syncs in same interval of a sync slot):
 * - two cliques may unwittingly be in sync (two MASTERS xmitting in same interval)
 * - merging: a Merger of a better clique may be xmitting sync FROM its sleeping slot to merge this clique into an offset syncSlot
 *
 */

#include <cassert>
#include "../log.h"
#include "../radioWrapper.h"
#include "syncAgent.h"
#include "schedule.h"

#ifdef OBS
void SyncAgent::onSyncWake() {
	// sync slot starts
	if ( !powerMgr->isPowerForRadio() ) {
		pauseSyncing();
		// assert App has a scheduled event; SyncAgent has no events scheduled
	}
	else {
		startSyncSlot();
		// assert receiver on and endSyncSlotTask is scheduled
	}

	// ensure endSyncSlotTask is scheduled or App wakingTask is scheduled
	// sleep
}
#endif


void SyncAgent::pauseSyncing() {
	/*
	 * Not enough power for self to continue syncing.
	 * Other units might still have power and assume mastership of my clique
	 */

	// assert radio not on

	// FUTURE if clique is probably not empty
	if (clique.isSelfMaster()) doDyingBreath();
	// else I am a slave, just drop out of clique

	// onSyncingPausedCallback();	// Tell app

	// ensure SyncAgent scheduled no event
	// ensure app has scheduled its own events (else will not wake from sleep)
}

void SyncAgent::doDyingBreath() {
	// Ask another unit in my clique to assume mastership.
	// Might not be heard.
	outwardSyncMsg.makeAbandonMastership(myID());
	xmit(&outwardSyncMsg);
}

// Scheduling

/*OBS
void SyncAgent::scheduleNextSyncRelatedTask() {
	// assert in syncSlot
	if (role.isMerger()) {
		// avoid collision
		if (cliqueMerger.shouldScheduleMerge()) {
			scheduleMergeWake();
		}
		else { scheduleSyncWake(); }
	}
	else {
		// Fish every period
		scheduleFishWake();
	}
	// assert some task scheduled
	// onSyncWake: start of next period
	// onMergeWake or onFishWake: in a normally-sleeping slot of this period
}


void SyncAgent::scheduleSyncWake() {
	// assert in work or fish or merge slot
	clique.schedule.scheduleStartSyncSlotTask(onSyncWake);
}
*/

#ifdef OBS
void SyncAgent::scheduleFishWake(){
	// assert in workSlot
	/*
	 * Schedule for random sleeping slot.
	 * Not to avoid collision of xmits, since fishing is receiving.
	 * Random to better cover time.
	 */
	clique.schedule.scheduleStartFishSlotTask(onFishWake);
}

void SyncAgent::scheduleMergeWake(){
	// Knows how to schedule mergeSlot at some time in current period
	// assert we have decided to send a mergeSync
	assert(cliqueMerger.isActive);
	clique.schedule.scheduleStartMergeSlotTask(onMergeWake, cliqueMerger.offsetToMergee);
}
#endif

void SyncAgent::startSyncSlot() {
	xmitRoleAproposSync();

	// even a Master listens for remainder of sync slot
	turnReceiverOn();	// turnReceiverOnWithCallback(onMsgReceivedInSyncSlot);
}

/*OBS
void SyncAgent::startSyncSlot() {
	// Start of sync slot coincident with start of period.
	clique.schedule.startPeriod();

	xmitRoleAproposSync();

	// even a Master listens for remainder of sync slot
	turnReceiverOnWithCallback(onMsgReceivedInSyncSlot);
	clique.schedule.scheduleEndSyncSlotTask(onSyncSlotEnd);
    // assert radio on
	// will wake on onMsgReceivedInSyncSlot or onSyncSlotEnd
	// sleep
*/


void SyncAgent::xmitRoleAproposSync() {
	// Assert self is in sync slot.

	// Only master xmits FROM its sync slot.
	if ( clique.isSelfMaster() && clique.masterXmitSyncPolicy.shouldXmitSync() ) {
		outwardSyncMsg.makeSync(myID());
		xmit(&outwardSyncMsg);
	}
}


void SyncAgent::endSyncSlot() {
	/*
	 * This may be late, when message receive thread this delays this.
	 * Also, there could be a race to deliver message with this event.
	 * FUTURE check for those cases.
	 * Scheduling of subsequent events does not depend on timely this event.
	 */

	// FUTURE we could do this elsewhere, e.g. start of sync slot
	if (dropoutMonitor.check()) {
		dropoutMonitor.heardSync();	// reset
		clique.onMasterDropout();
	}
}


#ifdef OBS
void SyncAgent::onSyncSlotEnd() {
	/*
	 * This may be late, when message receive thread this delays this.
	 * Also, there could be a race to deliver message with this event.
	 * FUTURE check for those cases.
	 * Scheduling of subsequent events does not depend on timely this event.
	 */

	// FUTURE we could do this elsewhere, e.g. start of sync slot
	if (dropoutMonitor.check()) {
		dropoutMonitor.heardSync();	// reset
		clique.onMasterDropout();
	}

	scheduleNextSyncRelatedTask();
	// workSlot follows syncSlot.  Fall into it.
	startWorkSlot();
	/*
	 * Assert onWorkSlotEnd scheduled
	 * AND some sync-related task is scheduled.
	 *
	 * assert radio on for work msgs
	 */
	// sleep
}
#endif


// SyncMessage handlers for messages received in sync slot

/*
 * Cases for sync messages:
 * 1. my cliques current master (usual)
 * 2. other clique master happened to start schedule coincident with my schedule
 * 3. other clique master clock drifts so schedules coincide
 * 4. member (master or slave) of other, better clique fished, caught my clique and is merging my clique
 * 5. a member of my clique failed to hear sync and is assuming mastership
 *
 * Cannot assert sender is a master (msg.masterID could be different from senderID)
 * Cannot assert self is slave
 * Cannot assert msg.masterID equals clique.masterID
 */
void SyncAgent::doSyncMsgInSyncSlot(SyncMessage* msg){
	// Cannot receive sync from self (xmitter and receiver are exclusive)

	if (isBetterSync(msg)) {
		clique.masterID = msg->masterID;
		// I might not be master anymore

		// Regardless who sent sync: is a valid heartbeat, I am synchronized
		dropoutMonitor.heardSync();

		// Regardless: from my master (small offset) or from another clique (large offset)
		clique.schedule.adjustBySyncMsg(msg);

		// FUTURE clique.historyOfMasters.update(msg);

		if (cliqueMerger.isActive) {
			// Already merging an other clique, now merge other clique to updated sync slot time
			cliqueMerger.adjustBySyncMsg(msg);
		}
	}
}

bool SyncAgent::isBetterSync(SyncMessage* msg){
	bool result = clique.isOtherCliqueBetter(msg->masterID);

	// Debug unusual, transient conditions
	if (!result){
		if (clique.isSelfMaster()) {
			// Sender has not heard my sync
			// Since I am still alive, they should not be assuming mastership.
			// Could be assymetric communication (I can hear they, they cannot hear me.)
			log("Worse sync while self is master.");
		}
		else { // self is slave
			// Sender has not heard my master's sync
			// My master may dropout, and they are assuming mastership.
			// Wait until I discover my master dropout
			// FUTURE: if msg.masterID < myID(), I should assume mastership instead of sender
			// FUTURE: if msg.masterID > myID() record msg.masterID in my historyOfMasters
			// so when I discover dropout, I will defer to msg.masterID
			log("Worse sync while self is slave.");
		}
	}

	return result;
}

void SyncAgent::doAbandonMastershipMsgInSyncSlot(SyncMessage* msg){
	// Master of my clique is abandoning
	tryAssumeMastership(msg);
}

void SyncAgent::tryAssumeMastership(SyncMessage* msg){
	/*
	 * My clique is still in sync, but master is dropout.
	 *
	 * Naive design: all units that hear master abandon assume mastership.
	 * FUTURE: keep historyOfMasters, and better slaves assume mastership.
	 */
	clique.masterID = myID();
	assert(clique.isSelfMaster());
}


void SyncAgent::doWorkMsgInSyncSlot(WorkMessage* msg){
	// Received in wrong slot, from out-of-sync clique
	/*
	 * Design decision: if work should only be done in sync with others, change this to ignore the msg.
	 */
	relayWorkToApp(msg);
	// FUTURE: treat this like fishing, we caught an out-of-sync clique
}


