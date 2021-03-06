
#include <cassert>

#include "fishSchedule.h"
#include "fishingManager.h"

#include "../../fishPolicy/fishingParameters.h"

#include "../../globals.h"  // clique
#include "../../logging/logger.h"
#include "../../clique/clique.h"

// radioSoC
#include <clock/clockDuration.h>




namespace {

// Calculated once, referenced often
LongTime _memoStartTimeOfFishSlot;
LongTime _memoEndTimeOfFishSlot;

} // namespace




void FishSchedule::setStartAndEndTimes() {
	memoTimeOfFishSessionStart();
	memoTimeOfFishSessionEnd();

	//Logger::log(" Fish:");
	//Logger::logInt(FishingManager::currentSessionStartSlotOrdinal());
	//Logger::log(" ");
	//Logger::log(_memoStartTimeOfFishSlot);
	//Logger::log(" ");
	//Logger::log(_memoEndTimeOfFishSlot);

}

DeltaTime FishSchedule::deltaToFishSessionStart(){
	return ClockDuration::clampedTimeDifferenceFromNow(_memoStartTimeOfFishSlot);
}

DeltaTime FishSchedule::deltaToFishSessionEnd(){
	// WAS return TimeMath::clampedTimeDifferenceFromNow(timeOfThisFishSlotEnd());
	return ClockDuration::clampedTimeDifferenceFromNow(_memoEndTimeOfFishSlot);
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
void FishSchedule::memoTimeOfFishSessionStart() {

	LongTime scheduledStartTime = FishingManager::getStartTimeToFish();
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
	LongTime clampedResult = clique.schedule.clampTimeBeforeLatestSlotMark(scheduledStartTime);

	if (clampedResult != scheduledStartTime)
		Logger::log("\nFish slot start too late");

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
	_memoStartTimeOfFishSlot = clampedResult;
}


void FishSchedule::memoTimeOfFishSessionEnd() {
	_memoEndTimeOfFishSlot = timeOfFishSessionEnd();
}


LongTime FishSchedule::timeOfFishSessionEnd() {
	/*
	 * Fish session duration from FishingManager, may vary.
	 * Next, clamp end time of long session started near end of sync period.
	 */
	LongTime scheduledEndTime = _memoStartTimeOfFishSlot
			+ FishingManager::getFishSessionDurationTicks();

	/*
	 * Fish session started near end of SyncPeriod
	 * should not end after end of SyncPeriod (next SyncPoint)
	 */
	LongTime clampedResult = clique.schedule.clampTimeBeforeLatestSlotMark(scheduledEndTime);
	if (clampedResult != scheduledEndTime)
			Logger::log("\nFish slot end too late");
	/*
	 * result may be < nowTime() i.e. in the past
	 * in which case subsequently computed delta will be 0 and sleepUntilTimeout(delta) will timeout immediately.
	 */
	return clampedResult;
}


void FishSchedule::logParameters() {
	Logger::log("Fish duration "); Logger::logInt(FishingManager::getFishSessionDurationTicks());
}
