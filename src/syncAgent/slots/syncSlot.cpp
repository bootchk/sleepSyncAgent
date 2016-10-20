
/*
 * THE sync slot of my schedule.
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
#include "../../platform/uniqueID.h"
#include "../syncAgent.h"

#include "../globals.h"
#include "syncSlot.h"

/*
 * Like other dispatchers, return true if heard message apropos to slot kind.
 *
 * If true, side effect is adjusting my sync.
 */
bool SyncSlot::dispatchMsgReceived(SyncMessage* msg) {
	// agnostic of role.isMaster or role.isSlave

	bool isFoundSyncKeepingMsg = false;

	switch(msg->type) {
	case MasterSync:
	case MergeSync:
		isFoundSyncKeepingMsg = doSyncMsg((SyncMessage*) msg);
		// Multiple syncs or sync

		// FUTURE discard other queued messages
		break;

	case AbandonMastership:
		doAbandonMastershipMsg((SyncMessage*) msg);
		break;

	case Work:
		doWorkMsg((WorkMessage*) msg);
		// FUTURE !!! msg is moved to work queue, not freed?
		break;
	}

	// FUTURE use handle and assert(msgHandle==nullptr);	// callee freed memory and nulled handle, or just nulled handle
	return isFoundSyncKeepingMsg;
}






/*
 * Transmit any sync in middle of slot.
 * Some literature refers to "guards" around the sync.
 * Syncing in middle has higher probability of being heard by drifted/skewed others.
 *
 * !!! The offset must be half the slot length, back to start of SyncPeriod
 */

void SyncSlot::perform() {
	startSyncSlot();	// radio on
	if (shouldTransmitSync())
		doMasterSyncSlot();
	else
		// isSlave or (isMaster and not xmitting (coin flip))
		doSlaveSyncSlot();
	endSyncSlot();
}

void SyncSlot::doMasterSyncSlot() {
	// Transmit sync in middle

	bool heardSyncKeepingSync = doMasterListenHalfSyncSlot(clique.schedule.deltaToThisSyncSlotMiddle);
	assert(radio->isDisabledState());

	if (heardSyncKeepingSync) {
		// I was Master, but just relinquished it
		assert(!clique.isSelfMaster());
		assert(!radio->isPowerOn());
		assert(radio->isDisabledState());	// not receiving
		doIdleSlotRemainder();
	}
	else {
		// Might have heard a sync from a worse clique
		sendMasterSync();
		// Keep listening for other better Masters.
		// Result doesn't matter, slot is over and we proceed whether we heard sync keeping sync or not.
		(void) doMasterListenHalfSyncSlot(clique.schedule.deltaToThisSyncSlotEnd);

		// assert radio on or off
	}
}


bool SyncSlot::doMasterListenHalfSyncSlot(OSTime (*timeoutFunc)()) {
	sleeper.clearReasonForWake();
	radio->receiveStatic();
	bool result = syncAgent.dispatchMsgUntil(
					dispatchMsgReceived,
					timeoutFunc
					);

	// assert radio is on or off
	return result;
}

// Sleep with radio off for remainder of sync slot
void SyncSlot::doIdleSlotRemainder() {
	assert(!radio->isPowerOn());
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisSyncSlotEnd());
}


void SyncSlot::doSlaveSyncSlot() {
	// listen for sync the whole period
	sleeper.clearReasonForWake();
	radio->receiveStatic(); // DYNAMIC (receiveBuffer, Radio::MaxMsgLength);
	// This assertion is time sensitive, can't stay in production code
	assert(!radio->isDisabledState()); // listening for other's sync
	// TODO not using result?
	syncAgent.dispatchMsgUntil(
				dispatchMsgReceived,
				clique.schedule.deltaToThisSyncSlotEnd);
}


void SyncSlot::startSyncSlot() {
	// common to Master and Slave SyncSlot
	radio->powerOnAndConfigure();
	radio->configureXmitPower(8);
	assert(radio->isPowerOn());
}


#ifdef Obsolete
void SyncSlot::xmitRoleAproposSync() {
	// Assert self is in sync slot.
	if (shouldTransmitSync()) {
		transmitMasterSync();
	}
}
#endif

bool SyncSlot::shouldTransmitSync() {
	// Only master xmits FROM its sync slot
	// and then with a coin-flip, for collision avoidance.
	return clique.isSelfMaster() && clique.masterXmitSyncPolicy.shouldXmitSync();
}


void SyncSlot::sendMasterSync() {
	log("Send MasterSync\n");
	makeCommonMasterSyncMessage();
	// assert common MasterSync message serialized into radio buffer
	radio->transmitStaticSynchronously();	// blocks until transmit complete
	// assert xmit is completed over-the-air
}


void SyncSlot::makeCommonMasterSyncMessage() {
	/*
	 * Make the common SyncMessage:
	 * - of type MasterSync
	 * - having forwardOffset unsigned delta now to next SyncPoint
	 */
	DeltaTime forwardOffset = clique.schedule.deltaNowToNextSyncPoint();
	// FUTURE include correction for latency (on receiver's end)

	// Since we are in sync slot near front of sync period, offset should (0, NormalSyncPeriodDuration)
	/*
	 * !!! Soft assertion susceptible to breakpoints.
	 * If breakpointed, nextSyncPoint is in past and forwardOffset is zero.
	 */
	// assert( forwardOffset > 0);

	assert( forwardOffset < clique.schedule.NormalSyncPeriodDuration);

	// FUTURE assert we are not xmitting sync past end of syncSlot?
	// i.e. calculations are rapid and sync slot not too short?

	serializer.outwardCommonSyncMsg.makeMasterSync(forwardOffset, myID());
	serializer.serializeOutwardCommonSyncMessage();
	assert(serializer.bufferIsSane());
}

void SyncSlot::endSyncSlot() {
	/*
	 * This may be late, when message receive thread delays this.
	 * Also, there could be a race to deliver message with this event.
	 * FUTURE check for those cases.
	 * Scheduling of subsequent events does not depend on timely this event.
	 */

	// Radio is on or off.  If on, we timeout'd while receiving
	if (radio->isPowerOn()) {
		radio->stopReceive();
		assert(radio->isDisabledState());	// receiveStatic() in next slot requires radio disabled.
	}
	// Radio still on or off
	// We don't bother to turn it off since work slot follows and needs radio on.

	// FUTURE we could do this elsewhere, e.g. start of sync slot so this doesn't delay the start of work slot
	if (!clique.isSelfMaster())
		checkMasterDroppedOut();

	// assert radio on or off
	// assert radio on => radio.isDisabledState()
}


void SyncSlot::checkMasterDroppedOut() {
	if (dropoutMonitor.check()) {
		log("Master dropped out\n");
		dropoutMonitor.reset();
		clique.onMasterDropout();
	}
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
bool SyncSlot::doSyncMsg(SyncMessage* msg){
	// assert SyncMsg is subtype MasterSync OR MergeSync
	// assert sync not from self (xmitter and receiver are exclusive)
	// assert self.isMaster || self.isSlave i.e. this code doesn't require any particular role

	bool doesMsgKeepSynch;

	// Most likely case first
	if (clique.isMyMaster(msg->masterID)) {
		// My Master could have fished another better clique and be MergeSyncing self
		// Each Sync has an adjustment, could be zero or small (MasterSync) or larger (MergeSync)
		log("Sync from master\n");
		clique.changeBySyncMessage(msg);
		dropoutMonitor.heardSync();
		doesMsgKeepSynch = true;
	}
	else if (isSyncFromBetterMaster(msg)) {
		// Strictly better
		log("Better master\n");
		changeMaster(msg);
		dropoutMonitor.heardSync();
		doesMsgKeepSynch = true;
	}
	else {
		/*
		 * Heard MasterSync in SyncSlot from other Master of other worse clique.
		 * OR heard MergeSync from Master or Slave of other worse clique.
		 * Master of my clique (could be self) should continue as Master.
		 * Don't tell other clique: since their sync slot overlaps with mine,
		 * they should eventually hear my clique master's sync and relinquish mastership.
		 */
		logWorseSync();
		// !!! SyncMessage does not keep me in sync: not dropoutMonitor.heardSync();
		doesMsgKeepSynch = false;
	}
	return doesMsgKeepSynch;
}


void SyncSlot::changeMaster(SyncMessage* msg) {
	// assert current slot is Sync
	assert(msg->masterID != clique.getMasterID());

	clique.changeBySyncMessage(msg);
	// assert endOfSyncPeriod was changed

	if (syncAgent.role.isMerger()) {
		// Already merging an other clique, now merge other clique to updated sync slot time
		syncAgent.cliqueMerger.adjustBySyncMsg(msg);
	}
}


bool SyncSlot::isSyncFromBetterMaster(SyncMessage* msg){
	bool result = clique.isOtherCliqueBetter(msg->masterID);
	return result;
}


void SyncSlot::logWorseSync() {
	// FUTURE: for now this is just logging, in future will record history
	if (clique.isSelfMaster()) {
		/*
		 * Sender has not heard my sync.
		 * Since I am still alive, they should not be assuming mastership.
		 * Could be assymetric communication (I can hear they, they cannot hear me.)
		 */
		log("Worse sync while self is master.\n");
	}
	else { // self is slave
		/*
		 * Sender has not heard my master's sync.
		 * My master may have dropped out (and I just don't know yet), and they are assuming mastership.
		 * Wait until I discover my master dropout.
		 */
		// FUTURE: if msg.masterID < myID(), I should assume mastership instead of sender
		// FUTURE: if msg.masterID > myID() record msg.masterID in my historyOfMasters
		// so when I discover dropout, I will defer to msg.masterID
		log("Worse sync while self is slave.\n");
	}
}


// Abandon mastership


void SyncSlot::doAbandonMastershipMsg(SyncMessage* msg){
	/*
	 * My clique is still in sync, but master is dropout.
	 *
	 * Naive design: all units that hear master abandon assume mastership.
	 * FUTURE: keep historyOfMasters, and better slaves assume mastership.
	 */
	(void) msg;  // FUTURE use msg to record history

	clique.setSelfMastership();
	assert(clique.isSelfMaster());
}



// Work in SyncSlot

void SyncSlot::doWorkMsg(WorkMessage* msg){
	// Received in wrong slot, from out-of-sync clique

	/*
	 * Design decision: if work should only be done in sync with others, change this to ignore the msg.
	 */
	syncAgent.relayWorkToApp(msg);

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
