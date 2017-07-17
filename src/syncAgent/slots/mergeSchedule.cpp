
#include <cassert>

#include "mergeSchedule.h"

#include "../modules/schedule.h"

/*
 * Merge slot:
 * Only start of slot is needed, not the end (slot ends when MergeSynce is xmitted.)
 *
 * offset comes from cliqueMerger.mergeOffset
 */
LongTime MergeSchedule::timeOfThisMergeStart(DeltaTime offset) {
	LongTime result;
	result = Schedule::startTimeOfSyncPeriod() + offset;
	assert(result < Schedule::endTimeOfSyncPeriod());
	return result;
}

DeltaTime MergeSchedule::deltaToThisMergeStart(const PeriodTime* const offset){
	return TimeMath::clampedTimeDifferenceFromNow(timeOfThisMergeStart(offset->get()));
}
