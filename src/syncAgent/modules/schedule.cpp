
#include <cassert>
#include "../../augment/random.h"

#include "schedule.h"


// static singleton data

LongClock Schedule::longClock;	// has-a


LongTime Schedule::startTimeOfSyncPeriod;

/*
 * !!!
 * Can change during current period.  See adjustBySyncMsg()
 * Can be in future, advances forward in time.
 */
LongTime Schedule::endTimeOfSyncPeriod;

// Remembered during fish slot
LongTime Schedule::memoStartTimeOfFishSlot;



void Schedule::startFreshAfterHWReset(){
	longClock.reset();
	startTimeOfSyncPeriod = longClock.nowTime();
	endTimeOfSyncPeriod = startTimeOfSyncPeriod + NormalSyncPeriodDuration;
	// Out of sync with other cliques
}


/*
 * "start" is verb: called at SyncPoint
 */
void Schedule::rollPeriodForwardToNow() {
	/*
	 * One begins where other ends.  See following comment.
	 */
	startTimeOfSyncPeriod = endTimeOfSyncPeriod;
	endTimeOfSyncPeriod = startTimeOfSyncPeriod + NormalSyncPeriodDuration;

	/*
	 * !!! This assertion can't be stepped-in while debugging
	 * since the RTC continues to run while you are stepping.
	 *
	 * assert startTimeOfSyncPeriod is close to nowTime().
	 * This is called at the time that should be SyncPoint.
	 * But since we can fish in the last slot before this time,
	 * and fishing may delay us a short time,
	 * this may be called a short time later than usual.
	 * If not a short time, is error in algorithm.
	 */
	//assert( longClock.timeDifferenceFromNow(startTimeOfSyncPeriod) < SlotDuration );
}


// Crux
void Schedule::adjustBySyncMsg(SyncMessage* msg) {
	/*
	 * A sync message adds to ***end*** of period (farther into the future)
	 *
	 *
	 * assert startTimeOfSyncPeriod < nowTime()  < endTimeOfSyncPeriod
	 * i.e. now in the current period, but may be near the end of it,
	 * or beyond it because of delay in processing fished SyncMessage?
	 *
	 * assert some task (endFishSlot) might be scheduled, but this doesn't affect that.
	 */

	assert(msg->deltaToNextSyncPoint < NormalSyncPeriodDuration);
	/*
	 * assert not much time has elapsed since msg arrived.
	 * I.E. nowTime() is approximate TOA of SyncMessage.
	 * For more accuracy, we could timestamp msg arrival as early as possible.
	 */
	endTimeOfSyncPeriod = longClock.nowTime() + msg->deltaToNextSyncPoint + NormalSyncPeriodDuration;

	// assert old startTimeOfSyncPeriod < new endTimeOfSyncPeriod  < nowTime() + 2*periodDuration
	// i.e. new endTimeOfSyncPeriod is within the span of old period or up to
}

#ifdef OBSOLETE
/*
 * Duration of this SyncPeriod, possibly as adjusted.
 */
DeltaTime Schedule::thisSyncPeriodDuration() {

}
#endif

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


DeltaTime  Schedule::deltaNowToNextSyncPoint() {
	return longClock.clampedTimeDifferenceFromNow(timeOfNextSyncPoint());
}
#ifdef OBSOLETE
// Different: backwards from others: from past time to now
DeltaTime  Schedule::deltaStartThisSyncPeriodToNow() {
	return longClock.clampedTimeDifference(longClock.nowTime(), startTimeOfSyncPeriod);
}
#endif

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


// Next
LongTime Schedule::timeOfNextSyncPoint() {
	return endTimeOfSyncPeriod;
}

// Start of period and start of syncSlot coincide.
// FUTURE: Choice of sync slot at start of period is arbitrary, allow it to be anywhere in period?
//LongTime Schedule::timeOfNextSyncSlotStart() { return timeOfNextSyncPeriodStart(); }

LongTime Schedule::timeOfThisSyncSlotEnd() { return startTimeOfSyncPeriod + SlotDuration; }
LongTime Schedule::timeOfThisSyncSlotMiddle() { return startTimeOfSyncPeriod + (SlotDuration/2); }
LongTime Schedule::timeOfThisWorkSlotEnd() { return startTimeOfSyncPeriod + 2 * SlotDuration; }


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
	ScheduleCount randomSleepingSlotOrdinal = randUnsignedInt16(FirstSleepingSlotOrdinal-1, CountSlots-1);
	LongTime result = startTimeOfSyncPeriod +  randomSleepingSlotOrdinal * SlotDuration;

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
	 * else we won't start next sync period on time.
	 */
	LongTime nextSyncPoint = timeOfNextSyncPoint();
	assert(result <= (nextSyncPoint - SlotDuration + 10*MsgDurationInTicks));

	// !!! Memoize
	memoStartTimeOfFishSlot = result;

	return result;
}

LongTime Schedule::timeOfThisFishSlotEnd() {
	LongTime result = memoStartTimeOfFishSlot + SlotDuration;

	// A Fish slot can be the last slot
	// Fish slot should not end after next SyncPoint
	LongTime nextSyncPoint = timeOfNextSyncPoint();
	if (result > nextSyncPoint)
			result = nextSyncPoint;


	// Soft assertions.  Both susceptible to breakpoints in received message IRQ handler.
	// Already near end?  Soft assertion that calculations and interrupts did not delay us.
	// Experience shows result could be only 8 ticks past now.
	// But why?
	LongTime now = longClock.nowTime();
	assert(result > (now + 3*MsgDurationInTicks));
	return result;
}



// Merge slot: offset from cliqueMerger, and no slotEnd needed
LongTime Schedule::timeOfThisMergeStart(DeltaTime offset) { return startTimeOfSyncPeriod + offset; }

