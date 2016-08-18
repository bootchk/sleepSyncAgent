
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
#include "../os.h"	// log()
#include "../radioWrapper.h"
#include "syncAgent.h"
#include "schedule.h"


void SyncAgent::onSyncWake() {
	// sync slot starts
	if ( !powerMgr->isPowerForRadio() ) {
		loseSync();
		// assert App scheduled a wakingTask
	}
	else {
		maintainSyncSlot();
		// assert receiver on and endSyncSlotTask is scheduled
	}

	// ensure endSyncSlotTask is scheduled or App wakingTask is scheduled
	// sleep
}


void SyncAgent::loseSync() {
	// Not enough power for self to continue in sync
	// Other units might still have power and assume mastership of my clique

	// assert radio not on

	// TODO if clique is probably not empty
	if (clique.isSelfMaster()) {
		// With dying breath, ask another unit in my clique to assume mastership
		// Might not be heard.
		msg.makeAbandonMastership(myID());
		xmit(msg);
	}
	// else I am a slave, just drop out of clique

	// Let app schedule a wakeTask
	onSyncLostCallback();

	// ensure no sync related task is scheduled
	// ensure app has scheduled its own wakeTask (else will not wake from sleep)
}


// Scheduling


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
	// assert in syncSlot?
	// TODO next time is one period from now
	clique.schedule.scheduleStartSyncSlotTask(onSyncWake);
}

void SyncAgent::scheduleFishWake(){
	// assert in syncSlot
	/*
	 * Schedule for random sleeping slot.
	 * Random set of units covers entire period.
	 * Not to avoid collision of xmits, since fishing is receiving.
	 */
	// TODO random
	clique.schedule.scheduleStartFishSlotTask(onFishWake);
}

void SyncAgent::scheduleMergeWake(){
	// Knows how to schedule mergeSlot at some time in current period
	// assert we have decided to send a mergeSync
	// assert cliqueMerger.isActive()
	clique.schedule.scheduleStartMergeSlotTask(onMergeWake);
}


void SyncAgent::maintainSyncSlot() {
	xmitRoleAproposSync();
	// even a Master listens for remainder of sync slot
	turnReceiverOnWithCallback(onMsgReceivedInSyncSlot);
	clique.schedule.scheduleEndSyncSlotTask(onSyncSlotEnd);
    // assert radio on
	// will wake on onMsgReceivedInSyncSlot or onSyncSlotEnd
	// sleep
}


void SyncAgent::xmitRoleAproposSync() {
	// Assert self is in sync slot.

	// Only master xmits FROM its sync slot.
	if ( clique.isSelfMaster() && clique.masterXmitSyncPolicy.shouldXmitSync() ) {
		msg.makeSync(myID());
		xmit(msg);
	}
}


void SyncAgent::onMsgReceivedInSyncSlot(Message msg) {
	switch(msg.type) {
	case Sync:
		doSyncMsgInSyncSlot(msg);
		break;
	case AbandonMastership:
		doAbandonMastershipMsgInSyncSlot(msg);
		break;
	case Work:
		doWorkMsgInSyncSlot(msg);
		break;
	default:
		break;
	}
}


void SyncAgent::onSyncSlotEnd() {
	// end of sync slot
	// TODO This may be late, since message spanning this delays this, OR message spanning is received after end?

	// TODO we could do this elsewhere, e.g. start of sync slot
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

// Message handlers for messages received in sync slot

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
void SyncAgent::doSyncMsgInSyncSlot(Message msg){
	// Cannot receive sync from self (xmitter and receiver are exclusive)

	if (isBetterSync(msg)) {
		clique.masterID = msg.masterID;
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

bool SyncAgent::isBetterSync(Message msg){
	bool result = clique.isOtherCliqueBetter(msg.masterID);

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

void SyncAgent::doAbandonMastershipMsgInSyncSlot(Message msg){
	// Master of my clique is abandoning
	// TODO assume mastership?
}

void SyncAgent::doWorkMsgInSyncSlot(Message msg){
	// Msg received in wrong slot, from out-of-sync clique
	// TODO work
}


