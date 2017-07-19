
#include "fishPolicy.h"

#include "../fishingParameters.h"


namespace {

ScheduleCount simpleUpCounter = FishingParameters::FirstSlotOrdinalToFish;

void incrementCounterModuloSleepingSlots(ScheduleCount* counter) {
	(*counter)++;
	if (*counter > FishingParameters::LastSlotOrdinalToFish) {
		*counter = FishingParameters::FirstSlotOrdinalToFish;
	}
}

void decrementCounterModuloSleepingSlots(ScheduleCount* counter) {
	(*counter)--;
	if (*counter < FishingParameters::FirstSlotOrdinalToFish) {
		*counter = FishingParameters::LastSlotOrdinalToFish;
	}
}
}


ScheduleCount SimpleFishPolicy::nextFishSlotOrdinal() {
	ScheduleCount result;

	incrementCounterModuloSleepingSlots(&simpleUpCounter);
	result = simpleUpCounter;
	assert(result >= FishingParameters::FirstSlotOrdinalToFish && result <= FishingParameters::LastSlotOrdinalToFish);
	return result;
}









namespace {


/*
 * !!! upCounter initialized to  last slot, the first call after reset increments,
 * and the first result is FirstSleepingSlotOrdinal.
 */
	ScheduleCount upCounter = FishingParameters::LastSlotOrdinalToFish;
	ScheduleCount downCounter = FishingParameters::FirstSlotOrdinalToFish;
	bool direction = true;
}

// !!! This should be the same as above compile time initialization.
void SyncRecoveryFishPolicy::reset() {
	upCounter = FishingParameters::FirstSlotOrdinalToFish;
	downCounter = FishingParameters::LastSlotOrdinalToFish;
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

	assert(result >=FishingParameters::FirstSlotOrdinalToFish && result <= FishingParameters::LastSlotOrdinalToFish);
	return result;
}


