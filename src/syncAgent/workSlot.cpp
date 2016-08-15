/*
 * SyncAgent methods used during THE workSlot of my schedule.
 * See general notes at syncSlot
 *
 * There is no onWorkWake: workSlot follows immediately after syncSlot
 */

#include "syncAgent.h"


void SyncAgent::startWorkSlot() {
	// assert still in task onEndSyncSlot
	xmitAproposWork();
	turnReceiverOnWithCallback(onMsgReceivedInWorkSlot);
	clique.schedule.scheduleEndWorkSlotTask(onWorkSlotEnd);
	// TODO are other tasks also scheduled?
}


void SyncAgent::xmitAproposWork() {
	// Assert self is in work slot.

	// Other units might be contending
	if ( isQueuedWorkMsg() ) {
		// TODO get ms
		xmit(Work);
	}
}


void SyncAgent::onMsgReceivedInWorkSlot(Message msg){
	switch(msg.type) {
		case Sync:
			/* Unusual: Another clique's sync slot at same time as my work slot.
			 * For now, ignore.  Assume fishing will find this other clique, or clocks drift.
			 * Alternative: merge other clique from within former work slot?
			 * doSyncMsgInWorkSlot(msg);
			 */
			break;
		case AbandonMastership:
			/*
			 * Unusual: Another clique's sync slot at same time as my work slot.
			 * For now ignore.  ??? doAbandonMastershipMsgInWorkSlot(msg);
			 */
			break;
		case Work:
			// Usual: work message in sync with my clique.
			doWorkMsgInWorkSlot(msg);
			break;
		default:
			break;
	}
	// assert radio on
	// assert onWorkSlotEnd scheduled
	// sleep
}


void SyncAgent::onWorkSlotEnd(){
	turnReceiverOff();
	// assert a sync related task is scheduled e.g. fish, merge, or syncWake
	// sleep
}


void SyncAgent::doWorkMsgInWorkSlot(Message msg) {
	// Relay to app
	onWorkMsgCallback(msg);
}

