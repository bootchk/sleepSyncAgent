
#include <cassert>
#include "schedule.h"
#include "time.h"
#include "../os.h"



// TODO random

// static singleton data

LongClock Schedule::longClock;	// has-a

/*
 * Can change during current period.
 * Can be in future.
 */
LongTime Schedule::startTimeOfPeriod;




void Schedule::start(){
	longClock.reset();
	startTimeOfPeriod = longClock.nowTime();
	// Out of sync with other cliques
}


// Crux
void Schedule::adjustBySyncMsg(Message msg) {
	/*
	 * A sync message adjusts start of period forward by an offset
	 *
	 * assert offset < period length
	 * assert startTimeOfPeriod < nowTime  < startTimeOfPeriod + period length
	 * i.e. now in the current period, but may be near the end of it.
	 */
	// assert some task (endFishSlot) might be scheduled.

	// assert not much time has elapsed since msg arrived.
	// For more accuracy, we could timestamp msg arrival
	startTimeOfPeriod = longClock.nowTime() + msg.offset;
	// assert new startTimeOfPeriod < old startTimeOfPeriod + 2*period length
	// i.e. new startTimeOfPeriod is within the old current period or in the period following
	// ????
}



// Scheduling slots tasks

// End slots

void Schedule::scheduleEndSyncSlotTask(void callback()) {
	scheduleTask(callback, clampedTimeDifference(timeOfThisSyncSlotEnd(), longClock.nowTime())); }

void Schedule::scheduleEndWorkSlotTask(void callback()) {
	scheduleTask(callback, clampedTimeDifferenceFromNow(timeOfThisWorkSlotEnd())); }

void Schedule::scheduleEndFishSlotTask(void callback()) {}
void Schedule::scheduleEndMergeSlotTask(void callback()) {}

// Fixed start slots
void Schedule::scheduleStartSyncSlotTask(void callback()) {
	scheduleTask(callback, clampedTimeDifferenceFromNow(startTimeOfNextPeriod()));}
// Not defined: scheduleStartWork : Work slot follows sync without start callback


void Schedule::scheduleStartFishSlotTask(void callback()) {
	/*
	 * Chosen randomly from sleeping slots.
	 * Remember it, to schedule end.
	 */
	// Long
	scheduleTask(callback, clampedTimeDifferenceFromNow(startTimeOfNextPeriod()));

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
	scheduleTask(callback, time);
}


// Deltas
DeltaTime  Schedule::deltaNowToStartNextSync() { return clampedTimeDifferenceFromNow(timeOfNextSyncSlotStart()); }
DeltaTime  Schedule::deltaStartThisSyncToNow() { return clampedTimeDifference(longClock.nowTime(), startTimeOfPeriod); }



// Times
// These return future times, as long as called at appropriate instant
// and task processing times is smaller than slotDurations.
/*
 * Even if these return past times, the OS allows scheduling in the past,
 * and scheduled task executes immediately after current task finishes?
 */

LongTime Schedule::startTimeOfNextPeriod() { return startTimeOfPeriod + CountSlots * SlotDuration; }

LongTime Schedule::timeOfThisSyncSlotEnd() { return startTimeOfPeriod + SlotDuration; }
LongTime Schedule::timeOfThisWorkSlotEnd() { return startTimeOfPeriod + 2 * SlotDuration; }
LongTime Schedule::timeOfNextSyncSlotStart() { return startTimeOfNextPeriod(); }



// LongTime math

// Can be called if you are not sure laterTime is after earlierTime
DeltaTime Schedule::clampedTimeDifference(LongTime laterTime, LongTime earlierTime) {
	// Returns positive time difference or zero
	DeltaTime result;
	if (earlierTime > laterTime) result = 0;
	else result = laterTime - earlierTime;	// !!! Coerce to 32-bit
	assert(result >= 0);
	return result;
}

// Requires futureTime less than 32-bit from now
DeltaTime Schedule::clampedTimeDifferenceFromNow(LongTime futureTime) {
	return clampedTimeDifference(futureTime, longClock.nowTime());	// !!! coerce to 32-bit
}
