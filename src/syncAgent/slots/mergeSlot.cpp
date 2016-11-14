
#include <cassert>

#include "../globals.h"
#include "mergeSlot.h"



namespace {

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
	assert(role.isMerger());
	syncSleeper.sleepUntilTimeout(timeoutUntilMerge);
	// assert time aligned with middle of a mergee sync slots (same wall time as fished sync from mergee.)
	prepareRadioToTransmitOrReceive();
	syncSender.sendMergeSync();
	shutdownRadio();

	if (mergePolicy.checkCompletionOfMergerRole()){
		mergePolicy.restart();
		syncAgent.toFisherRole();
		// assert next SyncPeriod will schedule FishSlot
		assert(!role.isMerger());
		assert(role.isFisher());
	}
	// else continue in role Merger

	assert(!radio->isPowerOn());
}

