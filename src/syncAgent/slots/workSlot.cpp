

#include <cassert>

#include "../../platform/platform.h"

#include "../globals.h"
#include "workSlot.h"

#include "../logMessage.h"



// static methods
namespace {

void start() {
	log(LogMessage::WorkSlot);
}

void end(){
	radio->powerOff();
}

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
		/*
		 * join other clique, all my cohorts will hear same msg and likewise join other clique.
		 * Not to role Merger, since my cohorts will merge without my telling them to.
		 */
		clique.changeBySyncMessage(msg);
		// assert schedule changed
		// assert role unchanged
		// assert if role.isMerger(), cliqueMerger was adjusted and will send MergeSync at proper time.
		// TODO: abandon this work slot or be sure to calculate end based on original schedule
	}
	else {
		log(LogMessage::WorseMasterSync);
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
}

void doMergeSyncMsg(){
	/*
	 * Another clique thinks this is SyncSlot of some clique.
	 * For now, ignore it.
	 */
	// FUTURE some clique may be not in range of my clique, i.e. need SyncRelay here
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
	assert(msg->type == Work);
	syncAgent.relayWorkToApp(msg->workPayload());
}

// Send work from app to other units
void sendWork(){

	log(LogMessage::SendWork);
	assert(radio->isPowerOn());
	assert(workOutMailbox->isMail());
	WorkPayload workPayload = workOutMailbox->fetch();
	serializer.outwardCommonSyncMsg.makeWork(workPayload, clique.getMasterID());
	// assert common SyncMessage serialized into radio buffer
	radio->transmitStaticSynchronously();	// blocks until transmit complete
}

} // namespace





/*
 *  Listen entire period.
 *  Receive work OR sync (similar to fishing.)
 */
void WorkSlot::performReceivingWork(){
	radio->powerOnAndConfigure();
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
	// Sleep radio off for all but middle, when we send.
	// Since not listening, not fishing.

	assert(workOutMailbox->isMail());	// require work to send
	assert(!radio->isPowerOn());
	sleepUntilWorkSendingTime();
	radio->powerOnAndConfigure();
	// TODO send work randomly in subslot of work slot, contention
	sendWork();
	radio->powerOff();
	sleepUntilEndWorkSlot();
	assert(!radio->isPowerOn());
}



void WorkSlot::performWork() {
	assert(!radio->isPowerOn());

	/*
	 * Work slot follows sync slot with no delay.
	 * I.E. no sleeping until start of WorkSlot.
	 * I.E. nowTime() is already start of WorkSlot.
	 */

	start();

	/*
	 * Choose kind of WorkSlot.
	 * Sending and receiving work are mutually exclusive.
	 */
	if ( workOutMailbox->isMail() ) {
		performSendingWork();
	}
	else {
		performReceivingWork();
	}

	// assert nowTime() is at end of WorkSlot
	assert(!radio->isPowerOn());
}







bool WorkSlot::dispatchMsgReceived(SyncMessage* msg){
	switch(msg->type) {
	case MasterSync:
		log(LogMessage::MasterSync);
		doMasterSyncMsg(msg);
		break;
	case MergeSync:
		log(LogMessage::MergeSync);
		doMergeSyncMsg();
		break;
	case AbandonMastership:
		log(LogMessage::AbandonMastership);
		doAbandonMastershipMsg();
		break;
	case Work:
		// Usual: work message in sync with my clique.
		// For now, WorkMessage is subclass of SyncMessage
		log(LogMessage::Work);
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


