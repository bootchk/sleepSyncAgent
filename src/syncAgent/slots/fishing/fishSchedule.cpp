
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
 * - ends a constant duration after memoized start.
 *
 * startTimeOfFishSlot is calculated once, at end of sync slot, in FishSchedule.init()
 *
 * startTimeOfFishSlot could be in the past or beyond sync period
 * User of the value must ensure that a timeout calculated from it
 * is in [0, timeTilLastSleepingSlot]
 */
void FishSchedule::memoizeTimeOfThisFishSlotStart() {

	LongTime result = FishingManager::getStartTimeToFish();

	/*
	 * Since some cpu cycles have elapsed after end of previous slot,
	 * startTimeOfFishSlot can be < longClock->nowTime().
	 * In other words, the case where chosen slot (to fish in) is slot immediately following previous slot,
	 * and time to start fish slot is already past.
	 * In this case, the subsequently calculated timeout will be zero,
	 * and there will be no sleep.
	 */

#ifdef FUTURE
	??? Dubious code.
	/*
	 * Time to start fish slot must be no later than start time of last sleeping slot,
	 * else we won't start next sync period on time.
	 */
	LongTime nextSyncPoint = timeOfNextSyncPoint();
	assert(result <= (nextSyncPoint - ScheduleParameters::SlotDuration + 10*ScheduleParameters::MsgDurationInTicks));
#endif

	/*
	 * SyncPeriod is never shortened by adjustment.
	 * Hence result must be less than timeOfNextSyncPoint,
	 * else not enough time to perform a FishSlot without delaying end of SyncPeriod.
	 */
	// FIXME this is wrong, caller must enforce
	// FIXME this is wrong, must allow for HFXO rampup, i.e. not beyond nextSyncPoint - rampup.
	assert(result < clique.schedule.timeOfNextSyncPoint() );

	_memoStartTimeOfFishSlot = result;
}



LongTime FishSchedule::timeOfThisFishSlotEnd() {
	// TODO delegate to FishingManager ?
	LongTime result = _memoStartTimeOfFishSlot
			+ FishingParameters::FishSessionDuration;

	/*
	 * A Fish slot started near end of SyncPeriod
	 * should not end after end of SyncPeriod (next SyncPoint)
	 */
	// Fish slot
	LongTime nextSyncPoint = clique.schedule.timeOfNextSyncPoint();
	if (result > nextSyncPoint) {
		Logger::log("End fish slot past sync point\n");
		result = nextSyncPoint;
	}

	/*
	 * result may be < nowTime() i.e. in the past
	 * in which case subsequently computed delta will be 0 and sleepUntilTimeout(delta) will timeout immediately.
	 */
	return result;
}
