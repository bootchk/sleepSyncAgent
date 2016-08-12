
/*
 * SyncAgent methods used during THE sync slot of my schedule.
 * (sync slot is not a class.)
 * All private
 *
 * Each Clique is on a Schedule that includes a sync slot.
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
#include "syncAgent.h"
#include "message.h"
#include "schedule.h"
#include "os.h"


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

	sleep();
	// ensure endSyncSlotTask is scheduled or App wakingTask is scheduled
}


void SyncAgent::loseSync() {
	// Not enough power for self to continue in sync
	// Other units might still have power and assume mastership of my clique

	// assert radio not on

	// TODO if clique is probably not empty
	if (clique.isSelfMaster()) {
		// With dying breath, ask another unit in my clique to assume mastership
		// Might not be heard.
		xmit(AbandonMastership);
	}
	// else I am a slave, just drop out of clique

	// Let app schedule a wakeTask
	onSyncLostCallback();

	// ensure no sync related task is scheduled
	// ensure app has scheduled its own wakeTask (else will not wake from sleep)
}


void SyncAgent::scheduleSyncWake() {
	scheduleTask(onSyncWake);
}

void SyncAgent::scheduleNextSyncRelatedTask() {
	if (role.isMerger()) {
		// avoid collision
		if (cliqueMerger.shouldScheduleMerge()) { scheduleTask(onMergeWake); }
		else { scheduleTask(onSyncWake); }
	}
	else {
		// Fish every period
		scheduleTask(onFishWake);
	}
	// assert some task scheduled
	// onSyncWake: start of next period
	// onMergeWake or onFishWake: in a normally-sleeping slot of this period
}



void SyncAgent::maintainSyncSlot() {
	doRoleAproposSyncXmit();
	// even a Master listens for remainder of sync slot
	turnRadioOnWithCallback(onMsgReceivedInSyncSlot);
	scheduleTask(onSyncSlotEnd);
	sleep();
    // assert radio on
	// will wake on onMsgReceivedInSyncSlot or onSyncSlotEnd
}


void SyncAgent::doRoleAproposSyncXmit() {
	// Assert self is in sync slot.

	// Only master xmits FROM its sync slot.
	if ( clique.isSelfMaster() && clique.masterXmitSyncPolicy.shouldXmitSync() ) {
		xmit(Sync);
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



}

// Message handlers for messages received in sync slot

void SyncAgent::doSyncMsgInSyncSlot(Message msg){
	// Received sync msg in sync slot.
	// require self is master (rare!) OR self is slave (usually)
	// Cannot receive sync from self (xmitter and receiver are exclusive)

	if (clique.isSelfMaster()) {
		// rare!  Another clique sent sync in my sync slot


	}
	else {
		// usual: self isSlave
		// sync is from my master
		// OR from member of a usurping clique
		// OR from master of clique that happens to be in sync

		// TODO assume master is best?
	}

	// Regardless who sent sync: is a valid heartbeat, I am synchronized
	dropoutMonitor.heardSync();

	// Regardless: from my master (small offset) or from another clique (large offset)
	clique.schedule.adjustBySyncMsg(msg);

	// TODO clique.historyOfMasters.update(msg);

	if (cliqueMerger.isActive()) {
		// Already merging another clique, now merge to updated sync slot time
		cliqueMerger.adjustBySyncMsg(msg);
	}
}

void SyncAgent::doAbandonMastershipMsgInSyncSlot(Message msg){
	// Master of my clique is abandoning
	// TODO assume mastership?
}

void SyncAgent::doWorkMsgInSyncSlot(Message msg){
	// Msg received in wrong slot, from an out-of-sync clique
	// TODO
}

void SyncAgent::onFishWake() {
}

void SyncAgent::onMergeWake() {
}


