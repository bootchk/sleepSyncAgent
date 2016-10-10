/*
 * SyncAgent methods used during THE workSlot of my schedule.
 * See general notes at syncSlot
 *
 * There is no onWorkWake: workSlot follows immediately after syncSlot
 */

#include <cassert>

#include "../platform/mailbox.h"

#include "syncAgent.h"

// FUTURE should only be visible to WorkSlot class

bool SyncAgent::dispatchMsgReceivedInWorkSlot(Message* msg){
	bool foundDesiredMessage = false;

	switch(msg->type) {
	case MasterSync:
	case MergeSync:
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
		doWorkMsgInWorkSlot((WorkMessage*) msg);
		// FUTURE msg requeued, not freed
		foundDesiredMessage = true;
		break;
	}
	return foundDesiredMessage;
}



void SyncAgent::doWorkSlot() {
	// assert work slot follows sync slot with no delay
	assert(radio->isDisabledState());	// not xmit or rcv
	startWorkSlot();	// might include a xmit
	assert(!radio->isDisabledState());   // receiving other's work
	dispatchMsgUntil(
			dispatchMsgReceivedInWorkSlot,
			clique.schedule.deltaToThisWorkSlotEnd);
	assert(radio->isDisabledState());
	endWorkSlot();
	assert(!radio->isPowerOn());
}


void SyncAgent::startWorkSlot() {
	// Prior SyncSlot may have offed radio
	if (!radio->isPowerOn()) radio->powerOnAndConfigure();

	// Arbitrary design decision, xmit queued work at beginning of work slot
	// TODO work should be transmitted in middle, guarded
	xmitAproposWork();

	// Rcv work from others
	assert(radio->isDisabledState());
	sleeper.clearReasonForWake();
	radio->receiveStatic();	//DYNAMIC receiveBuffer, Radio::MaxMsgLength);
}


void SyncAgent::xmitAproposWork() {
	// Assert self is in work slot.

	// Other units might be contending
	// FUTURE transmit with flip of coin, or delayed randomly

	if ( isQueuedWorkMsgFromApp() ) {
		void * workPayload = unqueueWorkMsgFromApp();
		// FUTURE use payload to make on-air message
		(void) workPayload;
		serializer.outwardCommonWorkMsg.make();
		freeWorkMsg(workPayload);
		// assert common WorkMessage serialized into radio buffer
		radio->transmitStaticSynchronously();	// blocks until transmit complete
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
	//TODO FUTURE relayWork
	(void) msg;
}

