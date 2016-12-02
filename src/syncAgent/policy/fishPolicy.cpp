
#include "fishPolicy.h"
#include "../modules/schedule.h"

/*
 * Last sleeping slot is the last slot i.e. CountSlots
 */

namespace {

ScheduleCount counter = Schedule::FirstSleepingSlotOrdinal;

void incrementCounterModuloSleepingSlots(ScheduleCount* counter) {
	(*counter)++;
	if (*counter > Schedule::CountSlots) {
		*counter = Schedule::FirstSleepingSlotOrdinal;
	}
}

void decrementCounterModuloSleepingSlots(ScheduleCount* counter) {
	(*counter)--;
	if (*counter < Schedule::FirstSleepingSlotOrdinal) {
		*counter = Schedule::CountSlots;
	}
}
}


ScheduleCount SimpleFishPolicy::next() {
	ScheduleCount result;

	incrementCounterModuloSleepingSlots(&counter);
	result = counter;
	assert(result >= Schedule::FirstSleepingSlotOrdinal && result <= Schedule::CountSlots);
	return result;
}


namespace {
	ScheduleCount upCounter = Schedule::FirstSleepingSlotOrdinal;
	ScheduleCount downCounter = Schedule::CountSlots;
	bool direction = true;
}

ScheduleCount SyncRecoveryFishPolicy::next() {
	ScheduleCount result;

	if (direction) {
		incrementCounterModuloSleepingSlots(&upCounter);
		result = upCounter;
	}
	else {
		decrementCounterModuloSleepingSlots(&downCounter);
		result = downCounter;
	}

	direction = ! direction;	// reverse direction, i.e. counter to return next call

	assert(result >= Schedule::FirstSleepingSlotOrdinal && result <= Schedule::CountSlots);
	return result;
}

void SyncRecoveryFishPolicy::reset() {
	upCounter = Schedule::FirstSleepingSlotOrdinal;
	downCounter = Schedule::CountSlots;
	direction = true;
	// next generated ordinal will be first sleeping slot
}
