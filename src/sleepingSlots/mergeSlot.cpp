
#include "../merge/mergeSlot.h"

#include <cassert>

#include "../../modules/syncSender.h"
#include "../../syncAgentImp/syncAgentImp.h"

// For debugging
#include "../../logging/logger.h"
#include "../../sleepers/syncSleeperObs.h"
#include "../../syncAgentImp/state/phase.h"
#include "../../syncAgentImp/state/role.h"
#include "../merge/mergeSchedule.h"






// FIXME pure class
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
	assert(MergerFisherRole::isMerger());

	Phase::set(PhaseEnum::SleepTilMerge);

	/*
	 * sleep without radio until MergeSlotStartTime.
	 * Pass to SyncSleeper a function to calculate MergeSlotStartTime
	 */
	SyncSleeper::sleepUntilTimeout(MergeSchedule::deltaToThisMergeStart);

	/*
	 * MergeSlotStartTime.
	 * Start process of activating radio
	 * Sleeping for dead time.
	 */
	Ensemble::startup();

	/*
	 * MergeTransmitTime
	 * now() is aligned with middle of a mergee sync slots (same wall time as fished sync from mergee.)
	 */
	Phase::set(PhaseEnum::Merge);
	Logger::logTicksSinceStartSyncPeriod();
	SyncSender::sendMergeSync();	// Synchronous

	// Radio is not in use.  Ensemble::shutdown requires it
	Ensemble::shutdown();

	if (mergePolicy.checkCompletionOfMergerRole()){
		SyncAgentImp::stopMerger();
	}
	// else continue in role Merger
}

