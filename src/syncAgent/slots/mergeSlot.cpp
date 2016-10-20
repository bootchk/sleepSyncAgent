/* MergeSlot of my schedule.
 *
 * Every unit can merge (after fishing catches another clique.)
 */
#include <cassert>

//#include "../syncAgent.h"
#include "../globals.h"
#include "mergeSlot.h"

// to and from Merger Role



/*
 * Merge slot
 *
 * Unique subclass of slot:
 * - xmit only, slot is not full length of other slots.
 * - not aligned with normally sleeping slots
 */


MergePolicy MergeSlot::mergePolicy;

void MergeSlot::perform() {
	assert(!radio->isPowerOn());
	assert(syncAgent.role.isMerger());
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisMergeStart(
			syncAgent.cliqueMerger.getOffsetToMergee()));
	// assert time aligned with middle of a mergee sync slots (same wall time as fished sync from mergee.)
	sendMergeSync();

	if (mergePolicy.checkCompletionOfMergerRole()){
		mergePolicy.restart();
		syncAgent.toFisherRole();
		// assert next syncSlot will schedule fishSlot
		assert(!syncAgent.role.isMerger());
		assert(syncAgent.role.isFisher());
	}
	// else continue in role Merger

	assert(!radio->isPowerOn());
}


void MergeSlot::sendMergeSync() {
	radio->powerOnAndConfigure();
	radio->configureXmitPower(8);
	syncAgent.cliqueMerger.makeMergeSync(serializer.outwardCommonSyncMsg);
	serializer.serializeOutwardCommonSyncMessage();
	assert(serializer.bufferIsSane());
	log("Send MergeSync\n");
	radio->transmitStaticSynchronously();	// blocks until transmit complete
	radio->powerOff();
}


