/*
 * SyncAgent methods used during THE fishSlot of my schedule.
 * See general notes at syncSlot.
 *
 * Every unit can fish (if not busy merging.)
 * Fish for other cliques by listening.
 */

#include "syncAgent.h"


void SyncAgent::onFishWake() {
	turnReceiverOnWithCallback(onMsgReceivedInFishSlot);
	scheduleTask(onFishSlotEnd);
}



void SyncAgent::onMsgReceivedInFishSlot(Message msg){
	switch(msg.type) {
		case Sync:
			/*
			 * Intended catch: another clique's sync slot.
			 */

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
}


void SyncAgent::onFishSlotEnd(){
	turnReceiverOff();
	sleep();
	// assert syncWake task is scheduled
}

/*
void SyncAgent::doWorkMsgInFishSlot(Message msg) {
	// Relay to app
	onWorkMsgCallback(msg);
}
*/

