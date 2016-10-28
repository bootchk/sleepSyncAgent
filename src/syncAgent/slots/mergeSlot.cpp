
#include <cassert>

#include "../globals.h"
#include "mergeSlot.h"

#include "../logMessage.h"


namespace {

void sendMergeSync() {
	log(LogMessage::SendMergeSync);
	syncAgent.cliqueMerger.makeMergeSync(serializer.outwardCommonSyncMsg);
	serializer.serializeOutwardCommonSyncMessage();
	assert(serializer.bufferIsSane());
	radio->transmitStaticSynchronously();	// blocks until transmit complete
}


/*
 * Partial application:
 * Convert function of one arg into function of no args.
 *
 * A method that whenever called, returns time remaining until time to perform merge.
 */
DeltaTime timeoutUntilMerge() {
	return clique.schedule.deltaToThisMergeStart(
			syncAgent.cliqueMerger.getOffsetToMergee());
}

} // namespace



// static data member
MergePolicy MergeSlot::mergePolicy;


void MergeSlot::perform() {
	assert(!radio->isPowerOn());
	assert(syncAgent.role.isMerger());
	syncSleeper.sleepUntilTimeout(timeoutUntilMerge);
	// assert time aligned with middle of a mergee sync slots (same wall time as fished sync from mergee.)
	prepareRadioToTransmitOrReceive();
	sendMergeSync();
	shutdownRadio();

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

