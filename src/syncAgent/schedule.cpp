
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
	scheduleTask(callback, longClock.clampedTimeDifference(timeOfThisSyncSlotEnd(), longClock.nowTime())); }

void Schedule::scheduleEndWorkSlotTask(void callback()) {
	scheduleTask(callback, longClock.clampedTimeDifferenceFromNow(timeOfThisWorkSlotEnd())); }

void Schedule::scheduleEndFishSlotTask(void callback()) {}
void Schedule::scheduleEndMergeSlotTask(void callback()) {}

// Fixed start slots
void Schedule::scheduleStartSyncSlotTask(void callback()) {
	scheduleTask(callback, longClock.clampedTimeDifferenceFromNow(startTimeOfNextPeriod()));}
// Not defined: scheduleStartWork : Work slot follows sync without start callback

/*
 * Chosen randomly from sleeping slots.
 */
void Schedule::scheduleStartFishSlotTask(void callback()) {}

/*
 * Chosen according to CliqueMerger
 */
void Schedule::scheduleStartMergeSlotTask(void callback()) {}


// Deltas
DeltaTime  Schedule::deltaNowToStartNextSync() { return longClock.clampedTimeDifferenceFromNow(timeOfNextSyncSlotStart()); }
DeltaTime  Schedule::deltaStartThisSyncToNow() { return longClock.clampedTimeDifference(longClock.nowTime(), startTimeOfPeriod); }



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
