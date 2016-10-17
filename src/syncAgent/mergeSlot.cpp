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
	mergePolicy.restart();

	// assert my schedule might have been adjusted
	assert(role.isMerger());
	ledLogger.toggleLED(3);
}

void SyncAgent::endMergerRole(){
	role.setFisher();
	// role does not know about cliqueMerger
	cliqueMerger.deactivate();
	ledLogger.toggleLED(3);
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
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisMergeStart(cliqueMerger.getOffsetToMergee()));
	// assert time aligned with middle of a mergee sync slots (same wall time as fished sync from mergee.)
	sendMergeSync();

	if (mergePolicy.checkCompletionOfMergerRole()){
		endMergerRole();
		// assert next syncSlot will schedule fishSlot
		assert(!role.isMerger());
		assert(role.isFisher());
	}
	// else continue in role Merger

	assert(!radio->isPowerOn());
}


void SyncAgent::sendMergeSync() {
	radio->powerOnAndConfigure();
	cliqueMerger.makeMergeSync(serializer.outwardCommonSyncMsg);
	serializer.serializeOutwardCommonSyncMessage();
	assert(serializer.bufferIsSane());
	log("Send MergeSync\n");
	radio->transmitStaticSynchronously();	// blocks until transmit complete
	radio->powerOff();
}


