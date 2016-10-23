

#include <cassert>

// platform
#include <logger.h>
// FUTURE platform
#include "../../platform/mailbox.h"

#include "../globals.h"
#include "workSlot.h"



// static methods
namespace {

#ifdef FUTURE
void toWorkMerger(SyncMessage* msg) {
	syncAgent.role.setWorkMerger();
	syncAgent.cliqueMerger.initFromMsg(msg);
}

void sleepUntilWorkMerger(){

	// Calculate timeout same as for any other merge
	sleeper.sleepUntilEventWithTimeout(
			clique.schedule.deltaToThisMergeStart(
						syncAgent.cliqueMerger.getOffsetToMergee()));
}

void xmitWorkMerger(){

}

#endif

void sleepUntilWorkSendingTime(){
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisWorkSlotMiddle());
}

void sleepUntilEndWorkSlot(){
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisWorkSlotEnd());
}



void startReceive() {
	// Prior SyncSlot may have offed radio
	if (!radio->isPowerOn()) {
		radio->powerOnAndConfigure();
		radio->configureXmitPower(8);
	}

	// Rcv work from others
	assert(radio->isDisabledState());
	sleeper.clearReasonForWake();
	radio->receiveStatic();	//DYNAMIC receiveBuffer, Radio::MaxMsgLength);
}

// Message handling




void doMasterSyncMsg(SyncMessage* msg){
	if (clique.isOtherCliqueBetter(msg->masterID)) {
		// join other clique, all my cohorts will also
		// TODO
	}
	else {
		// Other clique worse

		if (syncAgent.role.isMerger()) {
			// Already merging another clique into my clique.

		}
		else {
			// TODO assertrole is Fisher
			//toWorkMerger(msg);
		}
	}

	log("Heard MasterSync in work slot\n");

}
void doMergeSyncMsg(){
	/*
	 */
	log("Heard MergeSync in work slot\n");
}

void doAbandonMastershipMsg(){
	/*
	 * Unusual: Another clique's sync slot at same time as my work slot.
	 * The unit we heard is abandoning.
	 * There could be other units in its clique we want to merge.
	 * For now do nothing, and assume we will hear any the other units later.
	 */
}

void doWorkMsg(WorkMessage* msg) {
	syncAgent.relayWorkToApp(msg);
	// FUTURE msg requeued, not freed
	// TODO led log
}



} // namespace


void WorkSlot::performReceivingWork(){
	// Listen entire period
	startReceive();
	assert(!radio->isDisabledState());
	syncAgent.dispatchMsgUntil(
			dispatchMsgReceived,
			clique.schedule.deltaToThisWorkSlotEnd);
	assert(radio->isDisabledState());
	end();
	assert(!radio->isPowerOn());
}

/*
 * Xmit may not succeed (contention), is not acked.
 */
void WorkSlot::performSendingWork(){
	// assert is work queued
	assert(!radio->isPowerOn());
	sleepUntilWorkSendingTime();
	radio->powerOnAndConfigure();
	sendWork();
	radio->powerOff();
	sleepUntilEndWorkSlot();
	assert(!radio->isPowerOn());
}


// assert work slot follows sync slot with no delay


void WorkSlot::performWork() {
	assert(radio->isDisabledState());	// not xmit or rcv

	// Choose kind of WorkSlot
	if ( isQueuedWorkMsgFromApp() ) {
		performSendingWork();
	}
	else {
		performReceivingWork();
	}

	assert(!radio->isPowerOn());
}


#ifdef FUTURE
void WorkSlot::performWorkMerger() {
	// Act as Merger because self caught another clique in prior WorkSlot
	// Sleep most of slot, only waking to merge.

	assert(radio->isDisabledState());	// not xmit or rcv

	sleepUntilWorkMerger();
	xmitWorkMerger();
	sleepUntilEndWorkSlot();

	start();
	assert(!radio->isDisabledState());   // receiving other's work
	syncAgent.dispatchMsgUntil(
			dispatchMsgReceived,
			clique.schedule.deltaToThisWorkSlotEnd);
	assert(radio->isDisabledState());
	end();
	assert(!radio->isPowerOn());
}
#endif




void WorkSlot::end(){
	radio->powerOff();
}


// TODO WorkMessage
bool WorkSlot::dispatchMsgReceived(SyncMessage* msg){
	switch(msg->type) {
	case MasterSync:
		doMasterSyncMsg(msg);
		break;
	case MergeSync:
		doMergeSyncMsg();
		break;
	case AbandonMastership:
		doAbandonMastershipMsg();
		break;
	case Work:
		// Usual: work message in sync with my clique.
		// TODO casting?
		doWorkMsg((WorkMessage*) msg);
		break;
	}

	// Since a WorkSlot is like a FishingSlot, it continues listening for entire slot
	return false;	// meaning: don't stop listening
}

#ifdef OBS
void WorkSlot::xmitAproposWork() {
	// Assert self is in work slot.

	// Other units might be contending
	// FUTURE transmit with flip of coin, or delayed randomly

	if ( isQueuedWorkMsgFromApp() ) {
		xmitWork();
	}
}
#endif

void WorkSlot::sendWork(){
	void * workPayload = unqueueWorkMsgFromApp();
	// FUTURE use payload to make on-air message
	(void) workPayload;
	// FUTURE serializer.outwardCommonWorkMsg.make();
	freeWorkMsg(workPayload);
	// assert common WorkMessage serialized into radio buffer
	radio->transmitStaticSynchronously();	// blocks until transmit complete
}






