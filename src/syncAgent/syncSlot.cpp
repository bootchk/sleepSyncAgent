
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

#ifdef OLDDESIGN

void SyncAgent::doSyncSlot() {
	// Here, we xmit sync at beginning of slot
	startSyncSlot();
	xmitRoleAproposSync();
	// even a Master listens for remainder of sync slot
	radio->receiveStatic(); // DYNAMIC (receiveBuffer, Radio::MaxMsgLength);
	// race to sleep

	assert(!radio->isDisabled()); // listening for other's sync
	dispatchMsgUntil(
			dispatchMsgReceivedInSyncSlot,
			clique.schedule.deltaToThisSyncSlotEnd);
	endSyncSlot();
}

#endif

/*
 * Transmit any sync in middle of slot.
 * Some literature refers to "guards" around the sync.
 * Syncing in middle has higher probability of being heard by drifted/skewed others.
 *
 * !!! The offset must be half the slot length, back to start of SyncPeriod
 */

void SyncAgent::doSyncSlot() {
	startSyncSlot();	// radio on
	if (shouldTransmitSync())
		doMasterSyncSlot();
	else
		// isSlave or (isMaster and not xmitting (coin flip))
		doSlaveSyncSlot();
	endSyncSlot();
}

void SyncAgent::doMasterSyncSlot() {
	// Transmit sync in middle

	bool heardSyncKeepingSync = doMasterListenHalfSyncSlot(clique.schedule.deltaToThisSyncSlotMiddle);
	assert(radio->isDisabled());

	if (heardSyncKeepingSync) {
		// I was Master, but just relinquished it
		assert(!clique.isSelfMaster());
		// Leave radio powerOn (work slot requires it) but not receiving
		assert(radio->isDisabled());	// not receiving
		doIdleSlotRemainder();
	}
	else {
		// Might have heard a sync from a worse clique
		transmitMasterSync();
		// Keep listening for other better Masters
		(void) doMasterListenHalfSyncSlot(clique.schedule.deltaToThisSyncSlotEnd);
	}
}


bool SyncAgent::doMasterListenHalfSyncSlot(OSTime (*timeoutFunc)()) {
	radio->receiveStatic();
	bool result = dispatchMsgUntil(
					dispatchMsgReceivedInSyncSlot,
					timeoutFunc
					);
	return result;
}


void SyncAgent::doIdleSlotRemainder() {
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisSyncSlotEnd());
}


void SyncAgent::doSlaveSyncSlot() {
	// listen for sync the whole period
	radio->receiveStatic(); // DYNAMIC (receiveBuffer, Radio::MaxMsgLength);
	assert(!radio->isDisabled()); // listening for other's sync
	dispatchMsgUntil(
				dispatchMsgReceivedInSyncSlot,
				clique.schedule.deltaToThisSyncSlotEnd);
}


void SyncAgent::startSyncSlot() {
	// common to Master and Slave SyncSlot
	radio->powerOnAndConfigure();
	assert(radio->isPowerOn());
}


// Obsolete
void SyncAgent::xmitRoleAproposSync() {
	// Assert self is in sync slot.
	if (shouldTransmitSync()) {
		transmitMasterSync();
	}
}

bool SyncAgent::shouldTransmitSync() {
	// Only master xmits FROM its sync slot
	// and then with a coin-flip, for collision avoidance.
	return clique.isSelfMaster() && clique.masterXmitSyncPolicy.shouldXmitSync();
}

void SyncAgent::transmitMasterSync() {
	// Make SyncMessage having self as Master
	serializer.outwardCommonSyncMsg.makeSync(myID());
	xmitSync(serializer.outwardCommonSyncMsg);
}


void SyncAgent::endSyncSlot() {
	/*
	 * This may be late, when message receive thread delays this.
	 * Also, there could be a race to deliver message with this event.
	 * FUTURE check for those cases.
	 * Scheduling of subsequent events does not depend on timely this event.
	 */

	// FUTURE we could do this elsewhere, e.g. start of sync slot
	// so this doesn't delay the start of work slot
	if (dropoutMonitor.check()) {
		dropoutMonitor.heardSync();	// reset
		clique.onMasterDropout();
	}
	assert(radio->isPowerOn());

	// Radio can be receiving or not.
	// TODO only do this if we timedout (with receiver still enabled i.e. not isDisabled())

	radio->stopReceive();
	assert(radio->isDisabled());	// receiveStatic() in next slot requires radio disabled.
}





// Handlers for messages received in sync slot: Sync, AbandonMastership, Work




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

/*
 * Returns true if sync message keeps my sync (from current or new master of my clique.)
 */
bool SyncAgent::doSyncMsgInSyncSlot(SyncMessage* msg){
	// Cannot receive sync from self (xmitter and receiver are exclusive)

	// assert self.isMaster || self.isSlave i.e. this code doesn't require any particular role
	bool doesMsgKeepSynch;

	if (isSyncFromBetterMaster(msg)) {
		changeMaster(msg);

		// Regardless current master or new master sent sync: is a valid heartbeat, I am synchronized
		dropoutMonitor.heardSync();
		doesMsgKeepSynch = true;
	}
	else {
		/*
		 * Heard Sync in SyncSlot from other Master of other worse clique.
		 * Master of my clique (could be self) should continue as Master.
		 * Don't tell other clique: since their sync slot overlaps with mine,
		 * they should eventually hear my clique master's sync and relinquish mastership.
		 */
		// !!! SyncMessage does not keep me in sync: not dropoutMonitor.heardSync();
		doesMsgKeepSynch = false;
	}
	return doesMsgKeepSynch;
}


void SyncAgent::changeMaster(SyncMessage* msg) {
	assert(msg->masterID != clique.masterID);

	clique.masterID = msg->masterID;
	assert(!clique.isSelfMaster()); // even if I was before

	// FUTURE clique.historyOfMasters.update(msg);

	// Regardless: from my master (small offset) or from another clique (large offset)
	clique.schedule.adjustBySyncMsg(msg);

	if (cliqueMerger.isActive) {
		// Already merging an other clique, now merge other clique to updated sync slot time
		cliqueMerger.adjustBySyncMsg(msg);
	}
}


bool SyncAgent::isSyncFromBetterMaster(SyncMessage* msg){
	bool result = clique.isOtherCliqueBetter(msg->masterID);

	// This method should have no side effects, this is only debug transient conditions.
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




// Abandon mastership


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



// Work in SyncSlot

void SyncAgent::doWorkMsgInSyncSlot(WorkMessage* msg){
	// Received in wrong slot, from out-of-sync clique

	/*
	 * Design decision: if work should only be done in sync with others, change this to ignore the msg.
	 */
	relayWorkToApp(msg);

	/*
	 * FUTURE: treat this like fishing, we caught an out-of-sync clique.
	 *
	 * Design discussion:
	 * My SyncSlot aligns with other's WorkSlot => other's SyncSlot aligns with my last SleepingSlot.
	 * So eventually I would fish in my lastSleepingSlot and find other clique.
	 * But the merger comes sooner if we don't waste the info we just found.
	 * But it has complications:
	 * since I only heard a Work msg, I don't necessarily know whether other clique is better,
	 * (if Work messages don't contain MasterID and Offset?)
	 * I might need to schedule a fish for it in my last Sleeping slot.
	 */
}


