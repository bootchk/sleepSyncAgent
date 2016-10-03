/*
 * SyncAgent methods used during THE workSlot of my schedule.
 * See general notes at syncSlot
 *
 * There is no onWorkWake: workSlot follows immediately after syncSlot
 */

#include <cassert>

#include "../platform/mailbox.h"

#include "syncAgent.h"


void SyncAgent::doWorkSlot() {
	// assert work slot follows sync slot with no delay
	assert(radio->isDisabled());	// not xmit or rcv
	startWorkSlot();
	assert(!radio->isDisabled());   // receiving other's work
	dispatchMsgUntil(
			dispatchMsgReceivedInSyncSlot,
			clique.schedule.deltaToThisWorkSlotEnd);
	endWorkSlot();
	assert(!radio->isPowerOn());
}


void SyncAgent::startWorkSlot() {
	// assert still in task onEndSyncSlot
	assert(radio->isPowerOn());	// on at end sync slot, and work slot immediately follows
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
	// assert is receiving (we leave radio on for entire work slot?)
	radio->stopReceive();
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

