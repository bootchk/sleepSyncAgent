
#include <cassert>

//TODO no globals
#include "../globals.h"
#include "mergeSlot.h"

#include "../modules/syncSender.h"
#include "../sleepers/syncSleeper.h"
#include "../state/phase.h"
#include "../syncAgent.h"
#include "../scheduleParameters.h"



namespace {


/*
 * Partial application:
 * Convert function of one arg into function of no args.
 *
 * A method that whenever called, returns time remaining until time to start merge slot.
 * !!! This time is before actual start transmit end time, accounting for various preflight.
 */
DeltaTime timeoutUntilMergeSlotStart() {
	/*
	 * Much futzing here to use the constraints of MergeOffset type
	 */

	// Get DeltaTime from SyncPoint to when mergee should receive MergeSync
	DeltaTime deltaTime = SyncAgent::cliqueMerger.getPeriodTimeToMergeeSyncSlotMiddle()->get();

	// Adjust to get the time we should start transmit
	deltaTime -= ScheduleParameters::PreflightDelta;

	const MergeOffset deltaTimeToMergeSlotStart;
	// To type MergeOffset with constraints checked (insure modulo subtraction didn't yield a large DeltaTime.)
	deltaTimeToMergeSlotStart.set(deltaTime);

	// Pass address of stack var, its lifetime is long enough to pass as a parameter.
	return clique.schedule.deltaToThisMergeStart(&deltaTimeToMergeSlotStart);
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

	Phase::set(PhaseEnum::SleepTilMerge);
	//LogMessage::logMerge();

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
	logLongLong(LongClock::nowTime()); log(":mergeSync");
	Phase::set(PhaseEnum::Merge);
	SyncSender::sendMergeSync();	// Synchronous

	// Radio is not in use.  Ensemble::shutdown requires it
	Ensemble::shutdown();

	if (mergePolicy.checkCompletionOfMergerRole()){
		mergePolicy.restart();
		SyncAgent::toFisherRole();
		// assert next SyncPeriod will schedule FishSlot
		assert(!role.isMerger());
		assert(role.isFisher());
	}
	// else continue in role Merger
}

