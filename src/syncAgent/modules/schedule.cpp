
#include <cassert>
#include "../../platform/os.h"	// scheduleTask()
#include "../../platform/random.h"

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

#ifdef OBS
This obsolete design did not advance startTimeOfPeriod when power is low
void Schedule::resumeAfterPowerRestored(){
	// Roll period forward to nearest period boundary after nowTime()

	// Integer division.
	int periodsMissed = (longClock.nowTime() - startTimeOfPeriod) / SlotDuration;
	int rollTime = (periodsMissed + 1 ) * SlotDuration;
	startTimeOfPeriod = startTimeOfPeriod + rollTime;
	assert(startTimeOfPeriod > longClock.nowTime() );
	// caller will call scheduleStartSyncSlot()
}
#endif

void Schedule::startPeriod() {
	startTimeOfPeriod += SlotDuration;
	// Called when period should start.
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


#ifdef OBS
This obsolete design used a scheduler.  Currently using sleepUntil...
// Sync is first slot of next period
void Schedule::scheduleStartSyncSlotTask(void callback()) {
	scheduleTask(callback, clampedTimeDifferenceFromNow(startTimeOfNextPeriod()));}
void Schedule::scheduleEndSyncSlotTask(void callback()) {
	scheduleTask(callback, clampedTimeDifference(timeOfThisSyncSlotEnd(), longClock.nowTime())); }


void Schedule::scheduleEndWorkSlotTask(void callback()) {
	scheduleTask(callback, clampedTimeDifferenceFromNow(timeOfThisWorkSlotEnd())); }


void Schedule::scheduleStartFishSlotTask(void callback()) {
	/*
	 * Chosen randomly from sleeping slots.
	 * Remember it, to schedule end.
	 */
	startTimeOfFishSlot = startTimeOfPeriod + randInt(FirstSleepingSlotOrdinal-1, CountSlots-1) * SlotDuration;
	assert(startTimeOfFishSlot >= longClock.nowTime());
	assert(startTimeOfFishSlot <= startTimeOfNextPeriod());
	scheduleTask(callback, clampedTimeDifferenceFromNow(startTimeOfFishSlot));
}
void Schedule::scheduleEndFishSlotTask(void callback()) {
	// Require called during fish slot
	// i.e. assert startTimeOfFishSlot > nowTime > startTimeOfFishSlot + SlotDuration
	// else fish slot end is in the past.
	LongTime time = startTimeOfFishSlot + SlotDuration;
	assert(time >= longClock.nowTime());
	assert(time <= startTimeOfNextPeriod());
	scheduleTask(callback, clampedTimeDifferenceFromNow(time));
}


void Schedule::scheduleStartMergeSlotTask(void callback(), DeltaTime offset) {
	/*
	 * !!!Not like others, is not aligned with slots.
	 * Is scheduled within some usual sleepSlot of this period, but need not at start of slot.
	 * offset is from CliqueMerger.
	 */
	LongTime time = startTimeOfPeriod + offset;
	assert(time >= longClock.nowTime());
	assert(time <= startTimeOfNextPeriod());
	scheduleTask(callback, clampedTimeDifferenceFromNow(time));
}
#endif





/*
 * Deltas
 *
 * Uint32 math is modulo: a delta is correct if laterTime > earlierTime
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
LongTime Schedule::timeOfNextSyncPeriodStart() { return startTimeOfPeriod + PeriodDuration; }
LongTime Schedule::timeOfNextSyncSlotStart() { return timeOfNextSyncPeriodStart(); }

LongTime Schedule::timeOfThisSyncSlotEnd() { return startTimeOfPeriod + SlotDuration; }
LongTime Schedule::timeOfThisWorkSlotEnd() { return startTimeOfPeriod + 2 * SlotDuration; }
// Fish slot: starts at random time.  Ends after remembered start.
LongTime Schedule::timeOfThisFishSlotStart() {
	LongTime startTimeOfFishSlot = startTimeOfPeriod + randInt(FirstSleepingSlotOrdinal-1, CountSlots-1) * SlotDuration;
	assert(startTimeOfFishSlot >= longClock.nowTime());
	assert(startTimeOfFishSlot <= timeOfNextSyncPeriodStart());
	return startTimeOfFishSlot;
}
LongTime Schedule::timeOfThisFishSlotEnd() { return startTimeOfFishSlot + SlotDuration; }
// Merge slot: offset from cliqueMerger, and no slotEnd needed
LongTime Schedule::timeOfThisMergeStart(DeltaTime offset) { return startTimeOfPeriod + offset; }

