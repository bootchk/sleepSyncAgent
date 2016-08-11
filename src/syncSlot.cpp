
// SyncAgent methods for syncSlot

#include <cassert>
#include "syncAgent.h"
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
	// assert SyncAgent task is scheduled or App wakingTask is scheduled
	sleep();
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
}


void SyncAgent::doRoleAproposSyncXmit() {
	if ( clique.isSelfMaster() && syncPolicy.shouldXmitSync() ) {
		xmit(1 /*Sync*/ );
	}
}


void SyncAgent::onMsgReceivedInSyncSlot() {

}


void SyncAgent::onSyncSlotEnd() {

}
