
#include <cassert>
#include "schedule.h"
#include "time.h"
#include "../os.h"



// TODO adjust, random

// static singleton data
LongClock Schedule::longClock;
LongTime Schedule::startTimeOfPeriod;
/*
 * Can change during current period.
 * Can be in future.
 */



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
	DeltaTime deltaTime = longClock.clampedTimeDifference(
			timeOfThisSyncSlotEnd(),
			longClock.nowTime()
			);
	scheduleTask(callback, deltaTime );
}

void Schedule::scheduleEndWorkSlotTask(void callback()) {
	scheduleTask(callback,
			longClock.clampedTimeDifferenceFromNow(timeOfThisWorkSlotEnd())
			);
}

void Schedule::scheduleEndFishSlotTask(void callback()) {}
void Schedule::scheduleEndMergeSlotTask(void callback()) {}

// Start slots

void Schedule::scheduleStartSyncSlotTask(void callback()) {
	scheduleTask(callback,
				longClock.clampedTimeDifferenceFromNow(startTimeOfNextPeriod())
				);
}
// Not defined: scheduleStartWork : Work slot follows sync without start callback
void Schedule::scheduleStartFishSlotTask(void callback()) {}
void Schedule::scheduleStartMergeSlotTask(void callback()) {}


int Schedule::deltaNowToStartNextSync() {}
int Schedule::deltaStartThisSyncToNow() {}

// Times
// These are future, as long as called at appropriate instant and delays not exceed

LongTime Schedule::startTimeOfNextPeriod() {
	return startTimeOfPeriod + CountSlots * SlotDuration;
}

LongTime Schedule::timeOfThisSyncSlotEnd() {
	// Sync slot is first slot in period
	return startTimeOfPeriod;
};
LongTime Schedule::timeOfThisWorkSlotEnd() {
	// Work slot is second slot in period
	return startTimeOfPeriod + SlotDuration;
};

LongTime Schedule::timeOfNextSyncSlotStart() {
	return startTimeOfNextPeriod();
};
