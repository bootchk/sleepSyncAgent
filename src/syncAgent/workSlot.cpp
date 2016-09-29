/*
 * SyncAgent methods used during THE workSlot of my schedule.
 * See general notes at syncSlot
 *
 * There is no onWorkWake: workSlot follows immediately after syncSlot
 */

#include "../platform/mailbox.h"

#include "syncAgent.h"


void SyncAgent::startWorkSlot() {
	// assert still in task onEndSyncSlot
	radio->powerOnAndConfigure();
	xmitAproposWork();
	radio->receiveStatic();	//DYNAMIC receiveBuffer, Radio::MaxMsgLength);
	// OBS clique.schedule.scheduleEndWorkSlotTask(onWorkSlotEnd);
}


void SyncAgent::xmitAproposWork() {
	// Assert self is in work slot.

	// Other units might be contending
	if ( isQueuedWorkMsgFromApp() ) {
		void * workPayload = unqueueWorkMsgFromApp();
		// FUTURE use payload to make on-air message
		(void) workPayload;
		serializer.outwardCommonWorkMsg.make();
		freeWorkMsg(workPayload);
		xmitWork(serializer.outwardCommonWorkMsg);
	}
}



void SyncAgent::endWorkSlot(){
	radio->powerOff();
}


void SyncAgent::doWorkMsgInWorkSlot(WorkMessage* msg) {
	relayWorkToApp(msg);
}


void SyncAgent::relayWorkToApp(WorkMessage* msg) {
	/*
	 * FUTURE
	 * Alternatives are:
	 * - queue to worktask (unblock it)
	 * - onWorkMsgCallback(msg);  (callback)
	 */
}

