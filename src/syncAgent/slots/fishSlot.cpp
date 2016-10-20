/*
 * SyncAgent methods used during THE fishSlot of my schedule.
 * See general notes at syncSlot.
 *
 * Every unit can fish (if not busy merging.)
 * Fish for other cliques by listening.
 */

#include <cassert>

#include "../syncAgent.h"

// #include "../../platform/mailbox.h"


// FUTURE each type of slot should be its own class

bool SyncAgent::dispatchMsgReceivedInFishSlot(SyncMessage* msg){
	bool foundDesiredMessage;

	switch(msg->type) {
	case MasterSync:
		/*
		 * Intended catch: another clique's sync slot.
		 */
		doMasterSyncMsgInFishSlot(msg);
		// Stop listening: self can't handle more than one, or slot is busy with another merge
		foundDesiredMessage = true;
		break;
	case MergeSync:
		/*
		 * Unintended catch: Other (master or slave)
		 * is already xmitting into this time thinking it is SyncSlot of some third clique.
		 * Ignore except to stop fishing this slot.
		 */
		log("MergeSync in fish slot\n");
		foundDesiredMessage = true;
		break;
	case AbandonMastership:
		/*
		 * Unintended catch: Another clique's master is abandoning (exhausted power)
		 * For now ignore. Should catch clique again later, after another member assumes mastership.
		 */
		break;
	case Work:
		/*
		 * Unintended catch: Another clique's work slot.
		 * For now ignore. Should catch clique again later, when we fish earlier, at it's syncSlot.
		 * Alternative: since work slot follows syncSlot, could calculate syncSlot of catch, and merge it.
		 * Alternative: if work can be done when out of sync, do work.
		 */
		break;
	}

	return foundDesiredMessage;
}


void SyncAgent::doFishSlot() {
	// FUTURE: A fish slot need not be aligned with other slots, and different duration???

	// Sleep ultra low-power across normally sleeping slots to start of fish slot
	assert(!radio->isPowerOn());
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisFishSlotStart());


	startFishSlot();
	dispatchMsgUntil(
			dispatchMsgReceivedInFishSlot,
			clique.schedule.deltaToThisFishSlotEnd);
	assert(radio->isDisabledState());
	endFishSlot();
}


void SyncAgent::startFishSlot() {
	radio->powerOnAndConfigure();
	radio->configureXmitPower(8);
	sleeper.clearReasonForWake();
	radio->receiveStatic();		// DYNAMIC receiveBuffer, Radio::MaxMsgLength);
	// assert can receive an event that wakes imminently: race to sleep
}


void SyncAgent::endFishSlot(){
	/*
	 * Conditions:
	 * (no sync msg was heard and receiver still on)
	 * OR (heard a sync msg and (adjusted my schedule OR changed role to Merger))
	 *
	 * In case we adjusted my schedule, now is near its beginning.
	 * The next scheduled sync slot will be almost one full period from now.
	 *
	 * In case we adjusted changed role to Merger,
	 * first mergeSlot will be after next syncSlot.
	 */
	// radio might be off already
	radio->powerOff();
}


void SyncAgent::doMasterSyncMsgInFishSlot(SyncMessage* msg){
	toMergerRole(msg);
	// assert isMergerRole
	// assert (schedule changed AND self is merging my former clique)
	// OR (schedule unchanged AND self is merging other clique)
	// assert my schedule might have changed
}



/*
 * FUTURE act on it even though we are out of sync
void SyncAgent::doWorkMsgInFishSlot(SyncMessage msg) {
	// Relay to app
	onWorkMsgCallback(msg);
}
*/

