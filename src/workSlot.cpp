/*
 * SyncAgent methods used during THE workSlot of my schedule.
 * (work slot is not a class.)
 * All private
 */

//#include <cassert>
#include "syncAgent.h"
//#include "message.h"
//#include "schedule.h"
#include "os.h"

void SyncAgent::startWorkSlot() {
	xmitAproposWork();
	turnReceiverOnWithCallback(onMsgReceivedInWorkSlot);
	scheduleTask(onWorkSlotEnd);
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
			// Another clique's sync slot at same time as my work slot.
			// For now, ignore.  Assume fishing will find this other clique, or clocks drift.
			// Alternative: merge other clique from within former work slot?
			// doSyncMsgInWorkSlot(msg);
			break;
		case AbandonMastership:
			// Another clique's sync slot at same time as my work slot.
			// For now ignore.  ???
			// doAbandonMastershipMsgInWorkSlot(msg);
			break;
		case Work:
			doWorkMsgInWorkSlot(msg);
			break;
		default:
			break;
	}
}


void SyncAgent::onWorkSlotEnd(){
	turnReceiverOff();
	sleep();
	// assert a sync related task is scheduled e.g. fish, merge, or syncWake
}


void SyncAgent::doWorkMsgInWorkSlot(Message msg) {
	// Relay to app
	onWorkMsgCallback(msg);
}

