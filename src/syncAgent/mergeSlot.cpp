/* SyncAgent methods used during a mergeSlot of my schedule.
 * See general notes at syncSlot.
 *
 * Every unit can merge (after fishing catches another clique.)
 */
#include <cassert>

#include "syncAgent.h"

// to and from Merger Role

void SyncAgent::toMergerRole(SyncMessage* msg){
	// assert msg is master sync msg received in fishSlot
	assert( !cliqueMerger.isActive);
	assert(role.isFisher());
	role.setMerger();
	cliqueMerger.initFromMsg(msg);
	// assert my schedule might have been adjusted
	assert(cliqueMerger.isActive);
	assert(role.isMerger());
}

void SyncAgent::endMergerRole(){
	role.setFisher();
	// role does not know about cliqueMerger
	cliqueMerger.isActive = false;
}


// Merge slot

void SyncAgent::doMergeSlot() {
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisMergeStart(cliqueMerger.offsetToMergee));
	startMergeSlot();	// doMerge
	// Merge is xmit only, no sleeping til end of slot
	// Slot is not full length of other slots.
}


void SyncAgent::startMergeSlot() {
	/*
	 * xmit mergeSync
	 *
	 * Not aligned with my slots, aligned with mergee slots.
	 */
	assert(cliqueMerger.isActive);

	xmitSync(cliqueMerger.makeMergeSync(serializer.outwardCommonSyncMsg));

	/*
	FUTURE if multiple MergeSync xmits per merge
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
	endMergerRole();
	assert(!cliqueMerger.isActive);
	assert(role.isFisher());
}


