/*
 * SyncAgent methods used during THE fishSlot of my schedule.
 * See general notes at syncSlot.
 *
 * Every unit can fish (if not busy merging.)
 * Fish for other cliques by listening.
 */

#include "syncAgent.h"
#include "../radioWrapper.h"


void SyncAgent::onFishWake() {
	turnReceiverOnWithCallback(onMsgReceivedInFishSlot);
	clique.schedule.scheduleEndFishSlotTask(onFishSlotEnd);
}



void SyncAgent::onMsgReceivedInFishSlot(SyncMessage msg){
	switch(msg.type) {
		case Sync:
			/*
			 * Intended catch: another clique's sync slot.
			 */
			doSyncMsgInFishSlot(msg);
			// Self can't handle more than one, or slot is busy with another merge
			turnReceiverOff();
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
		default:
			break;
	}
	// assert endFishSlot is scheduled
	// sleep
}


void SyncAgent::onFishSlotEnd(){
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
	// not require receiver on
	turnReceiverOff();

	// syncSlot after fishSlot
	clique.schedule.scheduleStartSyncSlotTask(onSyncWake);
	// assert syncWake task is scheduled
	// sleep
}


void SyncAgent::doSyncMsgInFishSlot(SyncMessage msg){
	// heard a sync in a fishing slot
	if (msg.isOffsetSync()) {
		// Ignore: other clique is already merging
	}
	else {
		toMergerRole(msg);
	}
	// Assert msg ignored or isMergerRole
}



/*
 * TODO we might as well act on it even though we are out of sync
void SyncAgent::doWorkMsgInFishSlot(SyncMessage msg) {
	// Relay to app
	onWorkMsgCallback(msg);
}
*/

