
#include <cassert>

#include "../globals.h"
#include "mergeSlot.h"



// static data member
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
		// assert next SyncPeriod will schedule FishSlot
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


