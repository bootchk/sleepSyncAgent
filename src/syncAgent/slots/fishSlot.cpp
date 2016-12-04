/*
 * THE fishSlot of my schedule.
 *
 * Every unit can fish (if not busy merging.)
 * Fish for other cliques by listening.
 */

#include <cassert>

#include "../globals.h"
#include "fishSlot.h"
#include "../logMessage.h"


namespace {


bool doSyncMsg(SyncMessage* msg){
	/*
	 * MasterSync may be better or worse.
	 * toMergerRole() handles both cases,
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

} //namespace



/*
 * Intended catch: MasterSync from another clique's Master in its sync slot.
 */
bool FishSlot::doMasterSyncMsg(SyncMessage* msg){
	log(LogMessage::FishedMasterSync);
	return doSyncMsg(msg); }


/*
 * Unintended catch: Other (master or slave)
 * is already xmitting into this time thinking it is SyncSlot of some third clique.
 * Ignore except to stop fishing this slot.
 */
bool FishSlot::doMergeSyncMsg(SyncMessage* msg){
	log(LogMessage::FishedMergeSync);
	(void) msg; return true; }

/*
 * Intended catch: another clique's Master or Slave sending WorkSync in its sync slot.
 *
 * Implementation for combined Work/Sync slot.
 * Work carries sync, identifies master of clique and time of slot.
 *
 * Alternatively, when separate Work slot,
 * can calculate the other clique's sync slot from the Work msg.
 */
bool FishSlot::doWorkMsg(SyncMessage* msg) {
	log(LogMessage::FishedWorkSync);
	return doSyncMsg(msg);
}


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
 * Alternative designs:
 * Unintended catch: Another clique's work slot.
 * - Separate work slot: Ignore. Should catch clique again later, when we fish earlier, at it's syncSlot.
 * - Separate work slot:  Since work slot is in fixed relation to syncSlot, calculate syncSlot of catch, and merge it.
 * - Combined Work/Sync slot:
 *
 * In all alternatives: if work can be done when out of sync, do work.
 * onWorkMsgCallback(msg);	// Relay to app
 *
 *	Inherited behaviour from superclass is ignore.
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





