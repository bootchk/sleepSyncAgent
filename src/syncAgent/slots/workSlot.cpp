/*
 * THE workSlot of my schedule.
 * See general notes at syncSlot
 */

#include <cassert>

// platform
#include <logger.h>
// FUTURE platform
#include "../../platform/mailbox.h"

#include "../globals.h"
#include "workSlot.h"



void WorkSlot::perform() {
	// assert work slot follows sync slot with no delay
	assert(radio->isDisabledState());	// not xmit or rcv
	start();	// might include a xmit
	assert(!radio->isDisabledState());   // receiving other's work
	syncAgent.dispatchMsgUntil(
			dispatchMsgReceived,
			clique.schedule.deltaToThisWorkSlotEnd);
	assert(radio->isDisabledState());
	end();
	assert(!radio->isPowerOn());
}


void WorkSlot::start() {
	// Prior SyncSlot may have offed radio
	if (!radio->isPowerOn()) {
		radio->powerOnAndConfigure();
		radio->configureXmitPower(8);
	}

	// Arbitrary design decision, xmit queued work at beginning of work slot
	// FUTURE work should be transmitted in middle, guarded
	xmitAproposWork();

	// Rcv work from others
	assert(radio->isDisabledState());
	sleeper.clearReasonForWake();
	radio->receiveStatic();	//DYNAMIC receiveBuffer, Radio::MaxMsgLength);
}

void WorkSlot::end(){
	radio->powerOff();
}



bool WorkSlot::dispatchMsgReceived(SyncMessage* msg){
	bool foundDesiredMessage = false;

	switch(msg->type) {
	case MasterSync:
	case MergeSync:
		/* Unusual: Another clique's sync slot at same time as my work slot.
		 * For now, ignore.  Assume fishing will find this other clique, or clocks drift.
		 * Alternative: merge other clique from within former work slot?
		 * doSyncMsgInWorkSlot(msg);
		 */
		log("Heard MasterSync or MergeSync in work slot\n");
		break;
	case AbandonMastership:
		/*
		 * Unusual: Another clique's sync slot at same time as my work slot.
		 * For now ignore.  ??? doAbandonMastershipMsgInWorkSlot(msg);
		 */
		break;
	case Work:
		// Usual: work message in sync with my clique.
		doWorkMsg((WorkMessage*) msg);
		// FUTURE msg requeued, not freed
		foundDesiredMessage = true;
		break;
	}
	return foundDesiredMessage;
}


void WorkSlot::xmitAproposWork() {
	// Assert self is in work slot.

	// Other units might be contending
	// FUTURE transmit with flip of coin, or delayed randomly

	if ( isQueuedWorkMsgFromApp() ) {
		xmitWork();
	}
}


void WorkSlot::xmitWork(){
	void * workPayload = unqueueWorkMsgFromApp();
	// FUTURE use payload to make on-air message
	(void) workPayload;
	// FUTURE serializer.outwardCommonWorkMsg.make();
	freeWorkMsg(workPayload);
	// assert common WorkMessage serialized into radio buffer
	radio->transmitStaticSynchronously();	// blocks until transmit complete
}



void WorkSlot::doWorkMsg(WorkMessage* msg) {
	syncAgent.relayWorkToApp(msg);
}

