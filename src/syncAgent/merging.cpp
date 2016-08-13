/* SyncAgent methods used during a mergeSlot of my schedule.
 * See general notes at syncSlot.
 *
 * Every unit can merge (after fishing catches another clique.)
 */
#include <cassert>
#include "syncAgent.h"


void SyncAgent::toMergerRole(Message msg){
	// assert msg is masterSync msg received in fishSlot
	// assert otherClique not already in use
	assert(role.isFisher());
	role.setMerger();
	otherClique.initFromMsg(msg);
	if (clique.isOtherCliqueBetter(otherClique)){
		mergeMyClique();
	}
	else{
		mergeOtherClique();
	}
	assert(cliqueMerger.isActive());
	assert(role.isMerger());
	// assert will schedule syncSlot.
	// assert some future syncSlot will schedule onMergerWake
}

/*
 * These just set state in cliqueMerger.
 * At endSyncSlot, we schedule any mergeSlot.
 */
 
void SyncAgent::mergeMyClique(){
	/*
	 * Arrange state to start sending sync to my clique telling members to merge to other
	 * Self gets a new schedule.
	 * This time becomes a syncSlot in my new schedule.
	 * My old syncSlot becomes a mergeSlot in my new schedule.
	 */
	// TODO offset etc.
	cliqueMerger.setOffsetAndMasterID();	// TODO activateWith
}


void SyncAgent::mergeOtherClique(){
	// Start sending sync to other clique telling members to merge to self's clique
	// Self keeps old schedule.
	// This time becomes a mergeSlot in my new schedule.
	// TODO
}


void SyncAgent::onMergeWake() {
	// TODO construct MergeSync msg
	xmit(MergeSync);
	/*
	TODO if multiple MergeSync xmits per merge
	if (cliqueMerger.checkCompletionOfMergerRole()){
		assert(!cliqueMerger.isActive());
		role.setFisher();	// switch from merger to fisher
		// assert next syncSlot will schedule fishSlot
	}
	else {
		cliqueMerger.scheduleMergeWake();
	}
	*/
	
	// For now, only send one MergeSync per session of merger role
	role.setFisher();	// Completed merger role
	scheduleSyncWake();
	// sleep
}
