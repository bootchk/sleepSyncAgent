
#include <nRF5x.h>	// logger

#include "fishPolicy.h"
#include "../scheduleParameters.h"

/*
 * Last sleeping slot is the last slot i.e. CountSlots
 */

namespace {

ScheduleCount simpleUpCounter = ScheduleParameters::FirstSleepingSlotOrdinal;

void incrementCounterModuloSleepingSlots(ScheduleCount* counter) {
	(*counter)++;
	if (*counter > ScheduleParameters::CountSlots) {
		*counter = ScheduleParameters::FirstSleepingSlotOrdinal;
	}
}

void decrementCounterModuloSleepingSlots(ScheduleCount* counter) {
	(*counter)--;
	if (*counter < ScheduleParameters::FirstSleepingSlotOrdinal) {
		*counter = ScheduleParameters::CountSlots;
	}
}
}


ScheduleCount SimpleFishPolicy::next() {
	ScheduleCount result;

	incrementCounterModuloSleepingSlots(&simpleUpCounter);
	result = simpleUpCounter;
	assert(result >= ScheduleParameters::FirstSleepingSlotOrdinal && result <= ScheduleParameters::CountSlots);
	return result;
}









namespace {

/*
 * !!! upCounter initialized to  last slot, the first call after reset increments,
 * and the first result is FirstSleepingSlotOrdinal.
 */
	ScheduleCount upCounter = ScheduleParameters::CountSlots;
	ScheduleCount downCounter = ScheduleParameters::FirstSleepingSlotOrdinal;
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

	assert(result >= ScheduleParameters::FirstSleepingSlotOrdinal && result <= ScheduleParameters::CountSlots);
	return result;
}

void SyncRecoveryFishPolicy::reset() {
	log("reset FishPolicy\n");
	upCounter = ScheduleParameters::FirstSleepingSlotOrdinal;
	downCounter = ScheduleParameters::CountSlots;
	direction = true;
	// next generated ordinal will be first sleeping slot
}
