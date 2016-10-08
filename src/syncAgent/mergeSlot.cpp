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
	assert( role.isFisher());
	assert(role.isFisher());
	role.setMerger();
	cliqueMerger.initFromMsg(msg);

	// assert my schedule might have been adjusted
	assert(role.isMerger());
}

void SyncAgent::endMergerRole(){
	role.setFisher();
	// role does not know about cliqueMerger
	cliqueMerger.deactivate();
}


/*
 * Merge slot
 *
 * Unique subclass of slot:
 * - xmit only, slot is not full length of other slots.
 * - not aligned with normally sleeping slots
 */


void SyncAgent::doMergeSlot() {
	assert(!radio->isPowerOn());
	assert(role.isMerger());
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisMergeStart(cliqueMerger.offsetToMergee));
	// assert time aligned with middle of a mergee sync slots (same wall time as fished sync from mergee.)
	sendMerge();

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
	// FUTURE, send many
	endMergerRole();
	assert(!role.isMerger());
	assert(role.isFisher());
	assert(!radio->isPowerOn());
}


void SyncAgent::sendMerge() {
	radio->powerOnAndConfigure();
	xmitSync(cliqueMerger.makeMergeSync(serializer.outwardCommonSyncMsg));
	radio->powerOff();
}


