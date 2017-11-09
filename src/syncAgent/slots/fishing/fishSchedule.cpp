
#include <cassert>

#include "fishSchedule.h"

#include "../../globals.h"  // clique
#include "fishingParameters.h"
#include "fishingManager.h"

#include "../../logging/logger.h"

#include "../../clique/clique.h"

// FUTURE memoize end time also???



namespace {

// Remembered at start of fish slot
LongTime _memoStartTimeOfFishSlot;

} // namespace




void FishSchedule::initStartTime() {
	// Calculate the start once, memoize it
	memoizeTimeOfThisFishSlotStart();
}

DeltaTime FishSchedule::deltaToSlotStart(){
	return TimeMath::clampedTimeDifferenceFromNow(_memoStartTimeOfFishSlot);
}

DeltaTime FishSchedule::deltaToSlotEnd(){
	return TimeMath::clampedTimeDifferenceFromNow(timeOfThisFishSlotEnd());
}


/*
 * Fish slot:
 * - starts at slot normally sleeping (Trolling)
 * - starts in normally sleeping slot but not slot aligned (DeepFishing)
 * - ends a duration after memoized start.
 * (Intended duration is compile time constant but in future may be variable.)
 *
 * startTimeOfFishSlot is calculated once, at end of sync slot, in FishSchedule.init()
 */
void FishSchedule::memoizeTimeOfThisFishSlotStart() {

	LongTime result = FishingManager::getStartTimeToFish();
	/*
	 * result is wild, i.e. could be:
	 * - in past
	 * - too far in future:
	 * - - beyond end of current sync period
	 * - - beyond any other constraint on latest time to start fishing (HXFO startup for syncSlot)

	 * Result in past:
	 * Since some cpu cycles have elapsed after end of previous slot,
	 * startTimeOfFishSlot can be < longClock->nowTime().
	 * In other words, the case where chosen slot (to fish in) is slot immediately following previous slot,
	 * and time to start fish slot is already past.
	 * In this case, the subsequently calculated timeout will be zero,
	 * and there will be no sleep, and will fish slot will start immediately.

	 * Result too far in future:
	 * See Schedule::clampTimeBeforeLatestSlotMark() for explanation.
	 */
	result = clique.schedule.clampTimeBeforeLatestSlotMark(result);

	/*
	 * Only ensure start time.
	 * Not ensure end time will also meet these constraints.
	 * Ensure that later.
	 *
	 * Start time could be in past.
	 * User of start time must ensure that a timeout calculated from past start time is zero.
	 * User of start time probably should ensure that if the schedule changes,
	 * timeout to start time is still in range.
	 */
	_memoStartTimeOfFishSlot = result;
}



LongTime FishSchedule::timeOfThisFishSlotEnd() {
	/*
	 * Fish sessions are of fixed duration.
	 * Regardless of kind: Trolling or DeepFishing.
	 * Session may be many slots.
	 * FUTURE delegate to FishingManager so duration depends on kind.
	 */
	LongTime result = _memoStartTimeOfFishSlot
			+ FishingParameters::FishSessionDuration;

	/*
	 * A Fish slot started near end of SyncPeriod
	 * should not end after end of SyncPeriod (next SyncPoint)
	 */
	result = clique.schedule.clampTimeBeforeLatestSlotMark(result);

	/*
	 * result may be < nowTime() i.e. in the past
	 * in which case subsequently computed delta will be 0 and sleepUntilTimeout(delta) will timeout immediately.
	 */
	return result;
}
