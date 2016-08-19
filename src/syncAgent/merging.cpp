/* SyncAgent methods used during a mergeSlot of my schedule.
 * See general notes at syncSlot.
 *
 * Every unit can merge (after fishing catches another clique.)
 */
#include <cassert>
#include "../radioWrapper.h"
#include "syncAgent.h"


void SyncAgent::toMergerRole(Message msg){
	// assert msg is masterSync msg received in fishSlot
	assert( !cliqueMerger.isActive);
	assert(role.isFisher());
	role.setMerger();
	cliqueMerger.initFromMsg(msg);
	// TODO adjust my schedule, or has it already been done
	assert(cliqueMerger.isActive);
	assert(role.isMerger());
	// assert endFishSlot is scheduled
	// assert it will schedule syncSlot.
	// assert some future syncSlot will schedule onMergerWake
}


void SyncAgent::onMergeWake() {
	/*
	 * xmit mergeSync
	 *
	 * Not aligned with my slots, aligned with mergee slots.
	 */
	assert(cliqueMerger.isActive);

	cliqueMerger.makeMergeSync(msg);
	xmit(msg);

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
	scheduleSyncWake();
	// sleep
}

void SyncAgent::endMergerRole(){
	role.setFisher();
	// role does not know about cliqueMerger
	cliqueMerger.isActive = false;
}
