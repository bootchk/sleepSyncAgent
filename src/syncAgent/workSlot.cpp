/*
 * SyncAgent methods used during THE workSlot of my schedule.
 * See general notes at syncSlot
 *
 * There is no onWorkWake: workSlot follows immediately after syncSlot
 */

#include "../platform/os.h"
#include "../platform/radio.h"
#include "syncAgent.h"


void SyncAgent::startWorkSlot() {
	// assert still in task onEndSyncSlot
	xmitAproposWork();
	turnReceiverOn();	// WithCallback(onMsgReceivedInWorkSlot);
	// OBS clique.schedule.scheduleEndWorkSlotTask(onWorkSlotEnd);
}


void SyncAgent::xmitAproposWork() {
	// Assert self is in work slot.

	// Other units might be contending
	if ( isQueuedWorkOutMsg() ) {
		workMsg.make();
		xmitWork(workMsg);
	}
}





void SyncAgent::endWorkSlot(){
	turnReceiverOff();
	// assert a sync related task is scheduled e.g. fish, merge, or syncWake
	// TODO not true?  move scheduling here.
	// sleep
}


void SyncAgent::doWorkMsgInWorkSlot(WorkMessage* msg) {
	relayWorkToApp(msg);
}


void SyncAgent::relayWorkToApp(WorkMessage* msg) {
	// TODO queue to worktask
	//onWorkMsgCallback(msg);
}

