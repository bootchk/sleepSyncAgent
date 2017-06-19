
#include <cassert>

#include "../globals.h"
#include "mergeSlot.h"

#include "../modules/syncSender.h"



namespace {

/*
 * Partial application:
 * Convert function of one arg into function of no args.
 *
 * A method that whenever called, returns time remaining until time to perform merge.
 */
DeltaTime timeoutUntilMerge() {
	// TODO minor adjustment to account for ramp up of radio
	return clique.schedule.deltaToThisMergeStart(
			syncAgent.cliqueMerger.getOffsetToMergee());
}

} // namespace



// static data member
MergePolicy MergeSlot::mergePolicy;



void MergeSlot::tryPerform() {
	// XXX xmitting is low power, but check power anyway
	perform();
}

/*
 * Sleep all normally sleeping slots until time to xmit MergeSynce into mergee clique's SyncSlot.
 * !!! The time to xmit is not aligned with this schedule's slots, but with middle of mergee's SyncSlot.
 */
void MergeSlot::perform() {
	assert(Ensemble::isLowPower());
	assert(role.isMerger());

	phase = Phase::SleepTilMerge;
	//LogMessage::logMerge();

	// Hard sleep without listening until merge time
	// Pass to SyncSleeper a function to calculate merge time
	syncSleeper.sleepUntilTimeout(timeoutUntilMerge);

	// assert time aligned with middle of a mergee sync slots (same wall time as fished sync from mergee.)
	Ensemble::startup();
	logLongLong(clique.schedule.nowTime()); log(":mergeSync");
	phase = Phase::Merge;
	SyncSender::sendMergeSync();	// Synchronous
	assert(!Ensemble::isRadioInUse());
	Ensemble::shutdown();

	if (mergePolicy.checkCompletionOfMergerRole()){
		mergePolicy.restart();
		syncAgent.toFisherRole();
		// assert next SyncPeriod will schedule FishSlot
		assert(!role.isMerger());
		assert(role.isFisher());
	}
	// else continue in role Merger
}

