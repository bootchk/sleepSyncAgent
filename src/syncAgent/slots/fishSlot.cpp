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

void start() {
	log(LogMessage::FishSlot);
	radio->powerOnAndConfigure();
	radio->configureXmitPower(8);
	syncSleeper.clearReasonForWake();
	radio->receiveStatic();		// DYNAMIC receiveBuffer, Radio::MaxMsgLength);
	// assert can receive an event that wakes imminently: race to sleep
}


void end(){
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


void doMasterSyncMsg(SyncMessage* msg){
	/*
	 * MasterSync may be better or worse.  toMergerRole() handles both cases,
	 * but always toMerger(), either merging my clique or other clique.
	 */
	syncAgent.toMergerRole(msg);
	assert(syncAgent.role.isMerger());
	/*
	 * assert (schedule changed AND self is merging my former clique)
	 * OR (schedule unchanged AND self is merging other clique)
	 */
	// assert my schedule might have changed
}


bool dispatchMsgReceived(SyncMessage* msg){
	bool foundDesiredMessage;

	switch(msg->type) {
	case MasterSync:
		/*
		 * Intended catch: another clique's sync slot.
		 */
		log(LogMessage::MasterSync);
		doMasterSyncMsg(msg);
		// Stop listening: self can't handle more than one, or slot is busy with another merge
		foundDesiredMessage = true;
		break;
	case MergeSync:
		/*
		 * Unintended catch: Other (master or slave)
		 * is already xmitting into this time thinking it is SyncSlot of some third clique.
		 * Ignore except to stop fishing this slot.
		 */
		log(LogMessage::MergeSync);
		foundDesiredMessage = true;
		break;
	case AbandonMastership:
		/*
		 * Unintended catch: Another clique's master is abandoning (exhausted power)
		 * For now ignore. Should catch clique again later, after another member assumes mastership.
		 */
		log(LogMessage::AbandonMastership);
		break;
	case Work:
		/*
		 * Unintended catch: Another clique's work slot.
		 * For now ignore. Should catch clique again later, when we fish earlier, at it's syncSlot.
		 * Alternative: since work slot follows syncSlot, could calculate syncSlot of catch, and merge it.
		 * Alternative: if work can be done when out of sync, do work.
		 */
		log(LogMessage::Work);
		break;
	}

	return foundDesiredMessage;
}

} // namespace




void FishSlot::perform() {
	// FUTURE: A fish slot need not be aligned with other slots, and different duration???

	// Sleep ultra low-power across normally sleeping slots to start of fish slot
	assert(!radio->isPowerOn());
	syncSleeper.sleepUntilTimeout(clique.schedule.deltaToThisFishSlotStart);


	start();
	assert(radio->isPowerOn());
	assert(!radio->isDisabledState());
	syncSleeper.sleepUntilMsgAcceptedOrTimeout(
			dispatchMsgReceived,
			clique.schedule.deltaToThisFishSlotEnd);
	assert(radio->isDisabledState());
	end();
}







/*
 * FUTURE act on it even though we are out of sync
void FishSlot::doWorkMsg(SyncMessage msg) {
	// Relay to app
	onWorkMsgCallback(msg);
}
*/

