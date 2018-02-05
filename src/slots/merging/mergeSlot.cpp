
#include <cassert>

#include "mergeSlot.h"
#include "mergeSchedule.h"

#include "../../modules/syncSender.h"
#include "../../sleepers/syncSleeper.h"
#include "../../syncAgent/state/phase.h"
#include "../../syncAgent/syncAgent.h"
#include "../../scheduleParameters.h"

// For debugging
#include "../../syncAgent/state/role.h"
#include "../../logging/logger.h"



namespace {


/*
 * Partial application:
 * Convert function of one arg into function of no args.
 *
 * A method that whenever called, returns time remaining until time to start merge slot.
 * !!! This time is before actual start transmit end time, accounting for various preflight.
 */
DeltaTime timeoutUntilMergeSlotStart() {

	// Pass PeriodTime from my SyncPoint to when self should start MergeSlot
	return MergeSchedule::deltaToThisMergeStart(SyncAgent::cliqueMerger.getPeriodTimeToMergeSlotStart());
}

} // namespace



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
	SyncSleeper::sleepUntilTimeout(timeoutUntilMergeSlotStart);

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
		SyncAgent::stopMerger();
	}
	// else continue in role Merger
}

