/* SyncAgent methods used during THE mergeSlot of my schedule.
 * See general notes at syncSlot.
 *
 * Every unit can merge (after fishing catches another clique.)
 */

#include "syncAgent.h"


void SyncAgent::mergeMyClique(){
	// Start sending sync to my clique telling members to merge to other
	// Self gets a new schedule.
	// This time becomes a syncSlot in my new schedule.
	// My old syncSlot becomes a mergeSlot in my new schedule.
	cliqueMerger.setOffsetAndMasterID();
	scheduleTask(onMergeWake);
	// assert onMergeSlotWake could be scheduled for as soon as my old syncSlot
	// assert I will subsequently schedule onSyncWake one whole period from now
}


void SyncAgent::mergeOtherClique(){
	// Start sending sync to other clique telling members to merge to self's clique
	// Self keeps old schedule.
	// This time becomes a mergeSlot in my new schedule.
}

void SyncAgent::onMergeWake() {
	// Xmit sync
	sleep();
}
