
#include <cassert>
#include "../../augment/random.h"

#include "schedule.h"


// static singleton data

LongClock Schedule::longClock;	// has-a

/*
 * !!!
 * Can change during current period.  See adjustBySyncMsg()
 * Can be in future, advances forward in time.
 */
LongTime Schedule::startTimeOfPeriod;

// Remembered during fish slot
LongTime Schedule::startTimeOfFishSlot;



void Schedule::startFreshAfterHWReset(){
	longClock.reset();
	startTimeOfPeriod = longClock.nowTime();
	// Out of sync with other cliques
}


/*
 * "start" is verb: called when period should start.
 */
void Schedule::startPeriod() {
	startTimeOfPeriod += SlotDuration;
	// assert startTimeOfPeriod is close to nowTime()
	// ow we have missed a period or otherwise delayed unexpectedly.
}


// Crux
void Schedule::adjustBySyncMsg(SyncMessage* msg) {
	/*
	 * A sync message adjusts start of period forward by an offset
	 *
	 * assert offset < period length
	 * assert startTimeOfPeriod < nowTime  < startTimeOfPeriod + period length
	 * i.e. now in the current period, but may be near the end of it.
	 */
	// assert some task (endFishSlot) might be scheduled.

	// assert not much time has elapsed since msg arrived.
	// For more accuracy, we could timestamp msg arrival as early as possible.
	startTimeOfPeriod = longClock.nowTime() + msg->offset;
	// assert new startTimeOfPeriod < old startTimeOfPeriod + 2*period length
	// i.e. new startTimeOfPeriod is within the old current period or in the period following
	// ????
}


/*
 * Deltas
 *
 * uint32 math is modulo: a delta is correct if laterTime > earlierTime
 * (even if the uint32 clock has wrapped).
 * But is incorrect if earlierTime > laterTime.
 * So if you are calculating a timeout to a future deadline time,
 * and the deadline is already in the past, simple math on uint32 is wrong.
 * Use a LongClock to avoid the issue.
 *
 * All these involve a conversion to OSTime i.e. take lower 32 bits of result.
 *
 * If zero, a sleep...(0) executes immediately without delay.
 */
//DeltaTime  Schedule::deltaNowToStartNextSync() {
DeltaTime  Schedule::deltaNowToNextSyncPeriod() {
	return longClock.clampedTimeDifferenceFromNow(timeOfNextSyncSlotStart());
}
// Different: backwards from others: from past time to now
DeltaTime  Schedule::deltaStartThisSyncPeriodToNow() {
	return longClock.clampedTimeDifference(longClock.nowTime(), startTimeOfPeriod);
}
DeltaTime Schedule::deltaToThisSyncSlotEnd(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisSyncSlotEnd());
}
DeltaTime Schedule::deltaToThisSyncSlotMiddle(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisSyncSlotMiddle());
}
DeltaTime Schedule::deltaToThisWorkSlotEnd(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisWorkSlotEnd());
}
DeltaTime Schedule::deltaToThisFishSlotStart(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisFishSlotStart());
}
DeltaTime Schedule::deltaToThisFishSlotEnd(){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisFishSlotEnd());
}
DeltaTime Schedule::deltaToThisMergeStart(DeltaTime offset){
	return longClock.clampedTimeDifferenceFromNow(timeOfThisMergeStart(offset));
}

/*
 * Times
 *
 * These return future times, as long as called at appropriate instant
 * and task processing times is smaller than slotDurations.
 * When these return past times, calculation of DeltaTime clamps to zero.
 */
// Start of period and start of syncSlot coincide.
// FUTURE: Choice of sync slot at start of period is arbitrary, allow it to be anywhere in period.
LongTime Schedule::timeOfNextSyncPeriodStart() { return startTimeOfPeriod + SyncPeriodDuration; }
LongTime Schedule::timeOfNextSyncSlotStart() { return timeOfNextSyncPeriodStart(); }

LongTime Schedule::timeOfThisSyncSlotEnd() { return startTimeOfPeriod + SlotDuration; }
LongTime Schedule::timeOfThisSyncSlotMiddle() { return startTimeOfPeriod + (SlotDuration/2); }
LongTime Schedule::timeOfThisWorkSlotEnd() { return startTimeOfPeriod + 2 * SlotDuration; }


/*
 * Fish slot:
 * - starts at random slot normally sleeping.
 * - Ends after remembered start.
 */
/*
 * Start time is calculated at instant:  just after end of work slot.
 * Time til start is in [0, timeTilLastSleepingSlot]
 */
LongTime Schedule::timeOfThisFishSlotStart() {
	LongTime startTimeOfFishSlot = startTimeOfPeriod
			+ randUnsignedInt16(FirstSleepingSlotOrdinal-1, CountSlots-1) * SlotDuration;

	/*
	 * Since some cpu cycles have elapsed after end of work slot,
	 * startTimeOfFishSlot can be < longClock.nowTime().
	 * In other words, the case where randomly chosen fish slot is slot immediately following work slot,
	 * and time to start fish slot is already past.
	 * In this case, the subsequently calculated timeout will be zero,
	 * and there will be no sleep.
	 */

	/*
	 * Time to start fish slot must be no later than start time of last sleeping slot,
	 * else we won't start the next sync period on time.
	 */
	// TODO this is not tight enough, should be minus SlotDuration
	assert(startTimeOfFishSlot <= timeOfNextSyncPeriodStart());

	return startTimeOfFishSlot;
}

LongTime Schedule::timeOfThisFishSlotEnd() { return startTimeOfFishSlot + SlotDuration; }

// Merge slot: offset from cliqueMerger, and no slotEnd needed
LongTime Schedule::timeOfThisMergeStart(DeltaTime offset) { return startTimeOfPeriod + offset; }

