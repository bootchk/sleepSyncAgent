

#include <cassert>

#include "../../platform/platform.h"

#include "../globals.h"
#include "workSlot.h"



// static methods
namespace {



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
		// join other clique, all my cohorts will hear same msg and likewise join other clique.
		clique.changeBySyncMessage(msg);
	}
	else {
		/*
		 * Other clique worse.
		 *
		 * By duality, other clique will hear my work in their SyncSlot (if I send work often enough).
		 *
		 * This design gives responsibility for syncing to the other clique
		 * (when it hears my work in it's SyncSlot.)
		 *
		 * An alternative design is for this clique to xmit a MergeSync,
		 * either from WorkSlot or the normally sleeping slot just after
		 * (which would be into the WorkSlot of other clique.)
		 */
	}
#ifdef FUTURE
Alternative design: this cliques merges inferior clique.
		if (syncAgent.role.isMerger()) {
			// Already merging another clique into my clique.

		}
		else {
			// assertrole is Fisher
			//toWorkMerger(msg);
		}
	}
#endif

	log("Heard MasterSync in work slot\n");

}
void doMergeSyncMsg(){
	/*
	 * Another clique thinks this is SyncSlot of some clique.
	 * For now, ignore it.
	 */
	// TODO some clique may be not in range of my clique, i.e. need SyncRelay here
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


/*
 * Dual methods: to and from app
 */

// Pass work from other units to app
void doWorkMsg(SyncMessage* msg) {
	log("Heard work in work slot\n");
	assert(msg->type == Work);
	syncAgent.relayWorkToApp(msg->workPayload());
}

// Pass work from app to other units
void sendWork(){

	log("Send work from app\n");
	assert(workMailbox->isMail());
	WorkPayload workPayload = workMailbox->fetch();

	serializer.outwardCommonSyncMsg.makeWork(workPayload, clique.getMasterID());

	// assert common SyncMessage serialized into radio buffer
	radio->transmitStaticSynchronously();	// blocks until transmit complete
}

} // namespace






void WorkSlot::performReceivingWork(){
	// Listen entire period
	// Receive work OR sync (similar to fishing.)
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
	// Sleep radio off for all but middle, when we send
	// Since not listening, not fishing

	// assert is work queued
	assert(!radio->isPowerOn());
	sleepUntilWorkSendingTime();
	radio->powerOnAndConfigure();
	// TODO send work probabalistically in subslot of work slot, contention
	sendWork();
	radio->powerOff();
	sleepUntilEndWorkSlot();
	assert(!radio->isPowerOn());
}


// assert work slot follows sync slot with no delay
/*
 * Sending and receiving work are mutually exclusive.
 *
 * Contention to send means may fail.
 */

void WorkSlot::performWork() {
	assert(radio->isDisabledState());

	// Choose kind of WorkSlot
	if ( workMailbox->isMail() ) {
		performSendingWork();
	}
	else {
		performReceivingWork();
	}

	assert(!radio->isPowerOn());
}






void WorkSlot::end(){
	radio->powerOff();
}


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
		// For now, WorkMessage is subclass of SyncMessage
		doWorkMsg(msg);
		break;
	}

	// Since a WorkSlot is like a FishingSlot, it continues listening
	return false;	// meaning: don't stop listening until end of slot
}







#ifdef FUTURE

Possible code to perform merges from a WorkSlot


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


