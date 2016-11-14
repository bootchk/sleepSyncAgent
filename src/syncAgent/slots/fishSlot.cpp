/*
 * THE fishSlot of my schedule.
 *
 * Every unit can fish (if not busy merging.)
 * Fish for other cliques by listening.
 */

#include <cassert>

#include "../globals.h"
#include "fishSlot.h"



/*
 * Intended catch: another clique's sync slot.
 */
bool FishSlot::doMasterSyncMsg(SyncMessage* msg){
	/*
	 * MasterSync may be better or worse.  toMergerRole() handles both cases,
	 * but always toMerger(), either merging my clique or other clique.
	 */
	syncAgent.toMergerRole(msg);
	assert(role.isMerger());
	/*
	 * assert (schedule changed AND self is merging my former clique)
	 * OR (schedule unchanged AND self is merging other clique)
	 */
	// assert my schedule might have changed

	// Stop listening: self can't handle more than one, or slot is busy with another merge
	return true;
}

/*
 * Unintended catch: Other (master or slave)
 * is already xmitting into this time thinking it is SyncSlot of some third clique.
 * Ignore except to stop fishing this slot.
 */
bool FishSlot::doMergeSyncMsg(SyncMessage* msg){ (void) msg; return true; }



/*
 * AbandonMastership
 *
 * Unintended catch: Another clique's master is abandoning (exhausted power)
 * For now ignore. Should catch clique again later, after another member assumes mastership.
 *
 * Inherited behaviour from superclass.
 */

/*
 * Work
 *
 * Unintended catch: Another clique's work slot.
 * For now ignore. Should catch clique again later, when we fish earlier, at it's syncSlot.
 * Alternative: since work slot follows syncSlot, could calculate syncSlot of catch, and merge it.
 * Alternative: if work can be done when out of sync, do work.

 * FUTURE act on it even though we are out of sync
 *	// Relay to app
 *	onWorkMsgCallback(msg);
 *
 *	Inherited behaviour from superclass.
 */




void FishSlot::perform() {
	// FUTURE: A fish slot need not be aligned with other slots, and different duration???

	// Sleep ultra low-power across normally sleeping slots to start of fish slot
	assert(!radio->isPowerOn());
	syncSleeper.sleepUntilTimeout(clique.schedule.deltaToThisFishSlotStart);

	prepareRadioToTransmitOrReceive();
	startReceiving();
	// assert can receive an event that wakes imminently: race to sleep
	assert(radio->isPowerOn());
	assert(!radio->isDisabledState());
	syncSleeper.sleepUntilMsgAcceptedOrTimeout(
			//dispatchMsgReceived,
			this,
			clique.schedule.deltaToThisFishSlotEnd);
	assert(radio->isDisabledState());
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
	shutdownRadio();
}





