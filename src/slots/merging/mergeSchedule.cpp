
#include <cassert>

#include "mergeSchedule.h"

#include "../../clique/schedule.h"

#include "../../syncAgentImp/syncAgentImp.h"
#include "../../scheduleParameters.h"


/*
 * Merge slot:
 * Only start of slot is needed, not the end (slot ends when MergeSynce is xmitted.)
 *
 * offset comes from cliqueMerger.mergeOffset
 */
LongTime MergeSchedule::timeOfThisMergeStart(DeltaTime offset) {
	LongTime result;
	result = Schedule::startTimeOfSyncPeriod() + offset;

	// TODO could be stronger:  in the non-sync period: after sync slot and before end of unadjusted sync end
	assert(result < Schedule::endTimeOfSyncPeriod());
	return result;
}


DeltaTime MergeSchedule::deltaToThisMergeStart(const PeriodTime* const offset){
	return TimeMath::clampedTimeDifferenceFromNow(timeOfThisMergeStart(offset->get()));
}

/*
 * Partial application:
 * Convert function of one arg into function of no args.
 *
 * A method that whenever called, returns time remaining until time to start merge slot.
 * !!! This time is before actual start transmit end time, accounting for various preflight.
 */
DeltaTime MergeSchedule::deltaToThisMergeStart() {

	// Pass PeriodTime from my SyncPoint to when self should start MergeSlot
	return MergeSchedule::deltaToThisMergeStart(SyncAgentImp::cliqueMerger.getPeriodTimeToMergeSlotStart());
}





bool MergeSchedule::isMergerStartSyncPeriod() {
	/*
	 * Is period time of merge start within two RP start duration (two slot durations) of end of sync slot?
	 */
	const DeltaTime dt = SyncAgentImp::cliqueMerger.getPeriodTimeToMergeSlotStart()->get();
	bool result = dt <  ScheduleParameters::ThreeSlotDuration;
	return result;
}

bool MergeSchedule::isMergerEndSyncPeriod() {
	/*
	 * Is merge end within two RP start duration (two slot durations) of start of sync slot?
	 */
	const DeltaTime dt = SyncAgentImp::cliqueMerger.deltaMergeSlotEndToSyncPoint();
	bool result = dt <  ScheduleParameters::TwoSlotDuration;
	return result;
}

