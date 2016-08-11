
// SyncAgent methods for syncSlot

#include <cassert>
#include "syncAgent.h"
#include "message.h"
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
	// assert some SyncAgent task is scheduled or App wakingTask is scheduled
	sleep();
}

void SyncAgent::loseSync() {
	// Not enough power to continue in sync
	// Other units may still have power and be able to assume mastership

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

	// assert no sync related task is scheduled, but app has scheduled its own wakeTask
}

void SyncAgent::scheduleSyncWake() {
	scheduleTask(onSyncWake);
}


void SyncAgent::maintainSyncSlot() {
	doRoleAproposSyncXmit();
	listen(onMsgReceivedInSyncSlot);
	scheduleTask(onSyncSlotEnd);
	sleep();
    // assert radio on
	// will wake on onMsgReceivedInSyncSlot or onSyncSlotEnd
}


void SyncAgent::doRoleAproposSyncXmit() {
	// Assert self is in sync slot.
	// Only master xmits in sync slot.
	// Contention:
	//    two cliques may unwittingly by in sync
	//    merging: a better clique may be xmitting sync to merge this clique into an offset syncSlot
	if ( clique.isSelfMaster() && syncPolicy.shouldXmitSync() ) {
		xmit(1 /*Sync*/ );
	}
}


void SyncAgent::onMsgReceivedInSyncSlot(Message msg) {
	switch(msg.type) {
	case Sync:
		doSyncMsgInSyncSlot(msg);
		break;
	case AbandonMastership:
		break;
	case Work:
		break;
	default:
		break;
	}
}


void SyncAgent::onSyncSlotEnd() {

}

void SyncAgent::doSyncMsgInSyncSlot(Message msg){

}

