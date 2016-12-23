
#include <nRF5x.h>	// logger

#include "fishPolicy.h"
#include "../scheduleParameters.h"



namespace {
/*
 * Last sleeping slot could be the last slot i.e. CountSlots.
 * But we don't fish it, since it delays start of SyncPeriod.
 */

const ScheduleCount lastSlotToFish = ScheduleParameters::CountSlots - 1;	// !!!
const ScheduleCount firstSlotToFish = ScheduleParameters::FirstSleepingSlotOrdinal;



ScheduleCount simpleUpCounter = firstSlotToFish;

void incrementCounterModuloSleepingSlots(ScheduleCount* counter) {
	(*counter)++;
	if (*counter > lastSlotToFish) {
		*counter = firstSlotToFish;
	}
}

void decrementCounterModuloSleepingSlots(ScheduleCount* counter) {
	(*counter)--;
	if (*counter < firstSlotToFish) {
		*counter = lastSlotToFish;
	}
}
}


ScheduleCount SimpleFishPolicy::nextFishSlotOrdinal() {
	ScheduleCount result;

	incrementCounterModuloSleepingSlots(&simpleUpCounter);
	result = simpleUpCounter;
	assert(result >= firstSlotToFish && result <= lastSlotToFish);
	return result;
}









namespace {


/*
 * !!! upCounter initialized to  last slot, the first call after reset increments,
 * and the first result is FirstSleepingSlotOrdinal.
 */
	ScheduleCount upCounter = lastSlotToFish;
	ScheduleCount downCounter = firstSlotToFish;
	bool direction = true;
}

// !!! This should be the same as above compile time initialization.
void SyncRecoveryFishPolicy::reset() {
	log("reset FishPolicy\n");
	upCounter = firstSlotToFish;
	downCounter = lastSlotToFish;
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

	assert(result >=firstSlotToFish && result <= lastSlotToFish);
	return result;
}


