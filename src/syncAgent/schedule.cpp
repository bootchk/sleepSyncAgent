
#include <cassert>
#include "../os.h"	// scheduleTask()
#include "../random.h"

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

void Schedule::resumeAfterPowerRestored(){
	// Roll period forward to nearest period boundary after nowTime()

	// Integer division.
	int periodsMissed = (longClock.nowTime() - startTimeOfPeriod) / SlotDuration;
	int rollTime = (periodsMissed + 1 ) * SlotDuration;
	startTimeOfPeriod = startTimeOfPeriod + rollTime;
	assert(startTimeOfPeriod > longClock.nowTime() );
	// caller will call scheduleStartSyncSlot()
}

void Schedule::startPeriod() {
	// called by onStartSyncWake
	startTimeOfPeriod += SlotDuration;
	// assert startTimeOfPeriod is close to nowTime()
	// ow we have missed a period or otherwise delayed unexpectedly.
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
	// For more accuracy, we could timestamp msg arrival as early as possible.
	startTimeOfPeriod = longClock.nowTime() + msg.offset;
	// assert new startTimeOfPeriod < old startTimeOfPeriod + 2*period length
	// i.e. new startTimeOfPeriod is within the old current period or in the period following
	// ????
}



// Scheduling slots tasks, start and end


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
