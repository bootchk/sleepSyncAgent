
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
#include "../platform/mailbox.h"

#include "syncAgent.h"


bool SyncAgent::dispatchMsgReceivedInSyncSlot(Message* msg) {

	bool isFoundSyncKeepingMsg = false;

	switch(msg->type) {
	case MasterSync:
	case MergeSync:
		isFoundSyncKeepingMsg = doSyncMsgInSyncSlot((SyncMessage*) msg);
		// Multiple syncs or sync

		// FUTURE discard other queued messages
		break;

	case AbandonMastership:
		doAbandonMastershipMsgInSyncSlot((SyncMessage*) msg);
		break;

	case Work:
		doWorkMsgInSyncSlot((WorkMessage*) msg);
		// FUTURE !!! msg is moved to work queue, not freed?
		break;
	}

	// FUTURE use handle and assert(msgHandle==nullptr);	// callee freed memory and nulled handle, or just nulled handle
	return isFoundSyncKeepingMsg;
}



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
	// assert common SyncMessage serialized into radio buffer
	radio->transmitStaticSynchronously();	// blocks until transmit complete
}



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
	assert(radio->isDisabledState());

	if (heardSyncKeepingSync) {
		// I was Master, but just relinquished it
		assert(!clique.isSelfMaster());
		// Leave radio powerOn (work slot requires it) but not receiving
		assert(radio->isDisabledState());	// not receiving
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
	sleeper.clearReasonForWake();
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
	sleeper.clearReasonForWake();
	radio->receiveStatic(); // DYNAMIC (receiveBuffer, Radio::MaxMsgLength);
	// This assertion is time sensitive, can't stay in production code
	assert(!radio->isDisabledState()); // listening for other's sync
	dispatchMsgUntil(
				dispatchMsgReceivedInSyncSlot,
				clique.schedule.deltaToThisSyncSlotEnd);
}


void SyncAgent::startSyncSlot() {
	// common to Master and Slave SyncSlot
	radio->powerOnAndConfigure();
	assert(radio->isPowerOn());
}


#ifdef Obsolete
void SyncAgent::xmitRoleAproposSync() {
	// Assert self is in sync slot.
	if (shouldTransmitSync()) {
		transmitMasterSync();
	}
}
#endif

bool SyncAgent::shouldTransmitSync() {
	// Only master xmits FROM its sync slot
	// and then with a coin-flip, for collision avoidance.
	return clique.isSelfMaster() && clique.masterXmitSyncPolicy.shouldXmitSync();
}


void SyncAgent::transmitMasterSync() {
	makeCommonMasterSyncMessage();
	// assert common MasterSync message serialized into radio buffer
	radio->transmitStaticSynchronously();	// blocks until transmit complete
	// assert xmit is completed over-the-air
}


void SyncAgent::makeCommonMasterSyncMessage() {
	/*
	 * Make the common SyncMessage:
	 * - of type MasterSync
	 * - having forwardOffset unsigned delta now to next SyncPoint
	 */
	DeltaTime forwardOffset = clique.schedule.deltaNowToNextSyncPoint();
	// FUTURE include correction for latency (on receiver's end)

	// Since we are in sync slot near front of sync period, offset should (0, NormalSyncPeriodDuration)
	// !!! Soft.  Susceptible to breakpoints.
	assert( forwardOffset > 0);
	assert( forwardOffset < clique.schedule.NormalSyncPeriodDuration);

	// FUTURE assert we are not xmitting sync past end of syncSlot?
	// i.e. calculations are rapid and sync slot not too short?

	serializer.outwardCommonSyncMsg.makeMasterSync(forwardOffset, myID());
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
	// FUTURE avoid stopReceive unless we timedout (with receiver still enabled i.e. not isDisabled())

	radio->stopReceive();
	assert(radio->isDisabledState());	// receiveStatic() in next slot requires radio disabled.
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
	// assert SyncMsg is subtype MasterSync OR MergeSync

	// assert sync not from self (xmitter and receiver are exclusive)
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

	clique.changeBySyncMessage(msg);

	if (role.isMerger()) {
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
	(void) msg;  // FUTURE use msg to record history

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


