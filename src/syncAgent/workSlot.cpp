/*
 * SyncAgent methods used during THE workSlot of my schedule.
 * See general notes at syncSlot
 *
 * There is no onWorkWake: workSlot follows immediately after syncSlot
 */

#include <cassert>

#include "../platform/mailbox.h"

#include "syncAgent.h"

// TODO should only be visible to WorkSlot class

bool SyncAgent::dispatchMsgReceivedInWorkSlot(){
	bool foundDesiredMessage = false;
	//FUTURE Message* msg = serializer.unserialize(unqueueReceivedMsg());
	Message* msg = serializer.unserialize();
	if (msg != nullptr) {
		switch(msg->type) {
		case MasterSync:
		case MergeSync:
			/* Unusual: Another clique's sync slot at same time as my work slot.
			 * For now, ignore.  Assume fishing will find this other clique, or clocks drift.
			 * Alternative: merge other clique from within former work slot?
			 * doSyncMsgInWorkSlot(msg);
			 */
			freeReceivedMsg((void*) msg);
			break;
		case AbandonMastership:
			/*
			 * Unusual: Another clique's sync slot at same time as my work slot.
			 * For now ignore.  ??? doAbandonMastershipMsgInWorkSlot(msg);
			 */
			freeReceivedMsg((void*) msg);
			break;
		case Work:
			// Usual: work message in sync with my clique.
			doWorkMsgInWorkSlot((WorkMessage*) msg);
			// FUTURE msg requeued, not freed
			foundDesiredMessage = true;
			break;
		default:
			break;
		}
	}
	return foundDesiredMessage;
}



void SyncAgent::doWorkSlot() {
	// assert work slot follows sync slot with no delay
	assert(radio->isDisabled());	// not xmit or rcv
	startWorkSlot();
	assert(!radio->isDisabled());   // receiving other's work
	dispatchMsgUntil(
			dispatchMsgReceivedInWorkSlot,
			clique.schedule.deltaToThisWorkSlotEnd);
	endWorkSlot();
	assert(!radio->isPowerOn());
}


void SyncAgent::startWorkSlot() {
	// assert still in task onEndSyncSlot
	assert(radio->isPowerOn());	// on at end sync slot, and work slot immediately follows
	xmitAproposWork();
	assert(radio->isDisabled());
	sleeper.clearReasonForWake();
	radio->receiveStatic();	//DYNAMIC receiveBuffer, Radio::MaxMsgLength);
	// OBS clique.schedule.scheduleEndWorkSlotTask(onWorkSlotEnd);
}


void SyncAgent::xmitAproposWork() {
	// Assert self is in work slot.

	// Other units might be contending
	// TODO transmit with flip of coin, or delayed randomly

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

