
/*
 * SyncAgent methods used during THE sync slot of my schedule.
 *
 * General notes about slot implementations.
 * Slot is not a class.
 * All methods belong to SyncAgent, private.
 *
 * Each Clique is on a Schedule that includes a sync slot.
 * See general notes in Schedule.
 *
 * a) A clique's Master xmits sync FROM its sync slot.
 *
 * b) A Merger xmits sync FROM a sleeping slot INTO another clique's sync slot.
 *
 * c) One clique's sleeping slot may coincidentally be near in time to another clique's sleeping slot, or drift into each other.
 *
 * Contention (two syncs in same interval of a sync slot):
 * - situation c:
 *   two cliques may unwittingly be in sync (two MASTERS xmitting in same interval)
 * - situation b:
 *   merging: a Merger of a better clique may be xmitting sync FROM its sleeping slot to merge this clique into an offset syncSlot
 */

#include <cassert>
#include "../platform/log.h"
#include "../platform/uniqueID.h"
#include "syncAgent.h"




void SyncAgent::pauseSyncing() {
	/*
	 * Not enough power for self to continue syncing.
	 * Other units might still have power and assume mastership of my clique
	 */

	assert(!radio->isPowerOn());

	// FUTURE if clique is probably not empty
	if (clique.isSelfMaster()) doDyingBreath();
	// else I am a slave, just drop out of clique, others may have enough power

	// FUTURE onSyncingPausedCallback();	// Tell app
}


void SyncAgent::doDyingBreath() {
	// Ask another unit in my clique to assume mastership.
	// Might not be heard.
	serializer.outwardCommonSyncMsg.makeAbandonMastership(myID());
	xmitSync(serializer.outwardCommonSyncMsg);
}





void SyncAgent::startSyncSlot() {
	radio->powerOnAndConfigure();
	xmitRoleAproposSync();

	// even a Master listens for remainder of sync slot
	radio->receiveStatic(); // DYNAMIC (receiveBuffer, Radio::MaxMsgLength);
	// race to sleep
}



void SyncAgent::xmitRoleAproposSync() {
	// Assert self is in sync slot.

	// Only master xmits FROM its sync slot.
	if ( clique.isSelfMaster() && clique.masterXmitSyncPolicy.shouldXmitSync() ) {
		serializer.outwardCommonSyncMsg.makeSync(myID());
		xmitSync(serializer.outwardCommonSyncMsg);
	}
}


void SyncAgent::endSyncSlot() {
	/*
	 * This may be late, when message receive thread this delays this.
	 * Also, there could be a race to deliver message with this event.
	 * FUTURE check for those cases.
	 * Scheduling of subsequent events does not depend on timely this event.
	 */

	// FUTURE we could do this elsewhere, e.g. start of sync slot
	if (dropoutMonitor.check()) {
		dropoutMonitor.heardSync();	// reset
		clique.onMasterDropout();
	}
}





// SyncMessage handlers for messages received in sync slot

/*
 * Cases for sync messages:
 * 1. my cliques current master (usual)
 * 2. other clique master happened to start schedule coincident with my schedule
 * 3. other clique master clock drifts so schedules coincide
 * 4. member (master or slave) of other, better clique fished, caught my clique and is merging my clique
 * 5. a member of my clique failed to hear sync and is assuming mastership
 *
 * Cannot assert sender is a master (msg.masterID could be different from senderID)
 * Cannot assert self is slave
 * Cannot assert msg.masterID equals clique.masterID
 */
void SyncAgent::doSyncMsgInSyncSlot(SyncMessage* msg){
	// Cannot receive sync from self (xmitter and receiver are exclusive)

	if (isBetterSync(msg)) {
		clique.masterID = msg->masterID;
		// I might not be master anymore

		// Regardless who sent sync: is a valid heartbeat, I am synchronized
		dropoutMonitor.heardSync();

		// Regardless: from my master (small offset) or from another clique (large offset)
		clique.schedule.adjustBySyncMsg(msg);

		// FUTURE clique.historyOfMasters.update(msg);

		if (cliqueMerger.isActive) {
			// Already merging an other clique, now merge other clique to updated sync slot time
			cliqueMerger.adjustBySyncMsg(msg);
		}
	}
}

bool SyncAgent::isBetterSync(SyncMessage* msg){
	bool result = clique.isOtherCliqueBetter(msg->masterID);

	// Debug unusual, transient conditions
	if (!result){
		if (clique.isSelfMaster()) {
			// Sender has not heard my sync
			// Since I am still alive, they should not be assuming mastership.
			// Could be assymetric communication (I can hear they, they cannot hear me.)
			log("Worse sync while self is master.");
		}
		else { // self is slave
			// Sender has not heard my master's sync
			// My master may dropout, and they are assuming mastership.
			// Wait until I discover my master dropout
			// FUTURE: if msg.masterID < myID(), I should assume mastership instead of sender
			// FUTURE: if msg.masterID > myID() record msg.masterID in my historyOfMasters
			// so when I discover dropout, I will defer to msg.masterID
			log("Worse sync while self is slave.");
		}
	}

	return result;
}

void SyncAgent::doAbandonMastershipMsgInSyncSlot(SyncMessage* msg){
	// Master of my clique is abandoning
	tryAssumeMastership(msg);
}

void SyncAgent::tryAssumeMastership(SyncMessage* msg){
	/*
	 * My clique is still in sync, but master is dropout.
	 *
	 * Naive design: all units that hear master abandon assume mastership.
	 * FUTURE: keep historyOfMasters, and better slaves assume mastership.
	 */
	clique.masterID = myID();
	assert(clique.isSelfMaster());
}


void SyncAgent::doWorkMsgInSyncSlot(WorkMessage* msg){
	// Received in wrong slot, from out-of-sync clique
	/*
	 * Design decision: if work should only be done in sync with others, change this to ignore the msg.
	 */
	relayWorkToApp(msg);
	// FUTURE: treat this like fishing, we caught an out-of-sync clique
}


