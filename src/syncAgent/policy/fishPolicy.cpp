
#include <nRF5x.h>	// logger

#include "fishPolicy.h"
#include "../scheduleParameters.h"



namespace {




ScheduleCount simpleUpCounter = ScheduleParameters::FirstSlotOrdinalToFish;

void incrementCounterModuloSleepingSlots(ScheduleCount* counter) {
	(*counter)++;
	if (*counter > ScheduleParameters::LastSlotOrdinalToFish) {
		*counter = ScheduleParameters::FirstSlotOrdinalToFish;
	}
}

void decrementCounterModuloSleepingSlots(ScheduleCount* counter) {
	(*counter)--;
	if (*counter < ScheduleParameters::FirstSlotOrdinalToFish) {
		*counter = ScheduleParameters::LastSlotOrdinalToFish;
	}
}
}


ScheduleCount SimpleFishPolicy::nextFishSlotOrdinal() {
	ScheduleCount result;

	incrementCounterModuloSleepingSlots(&simpleUpCounter);
	result = simpleUpCounter;
	assert(result >= ScheduleParameters::FirstSlotOrdinalToFish && result <= ScheduleParameters::LastSlotOrdinalToFish);
	return result;
}









namespace {


/*
 * !!! upCounter initialized to  last slot, the first call after reset increments,
 * and the first result is FirstSleepingSlotOrdinal.
 */
	ScheduleCount upCounter = ScheduleParameters::LastSlotOrdinalToFish;
	ScheduleCount downCounter = ScheduleParameters::FirstSlotOrdinalToFish;
	bool direction = true;
}

// !!! This should be the same as above compile time initialization.
void SyncRecoveryFishPolicy::reset() {
	log("reset FishPolicy\n");
	upCounter = ScheduleParameters::FirstSlotOrdinalToFish;
	downCounter = ScheduleParameters::LastSlotOrdinalToFish;
	direction = true;
	// next generated ordinal will be first sleeping slot
}

ScheduleCount SyncRecoveryFishPolicy::nextFishSlotOrdinal() {
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

	assert(result >=ScheduleParameters::FirstSlotOrdinalToFish && result <= ScheduleParameters::LastSlotOrdinalToFish);
	return result;
}


