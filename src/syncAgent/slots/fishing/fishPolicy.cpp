
#include "fishPolicy.h"
#include "../../clique/clique.h"
#include "../../globals.h"   // clique
#include "../../logging/logger.h"
#include "utils.h"

using namespace Fishing;

namespace {
SlotCount simpleUpCounter = FishingParameters::FirstSlotOrdinalToFish;
}

SlotCount SimpleFishPolicy::nextFishSlotOrdinal() {
	SlotCount result;

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
	SlotCount upCounter = FishingParameters::LastSlotOrdinalToFish;
	SlotCount downCounter = FishingParameters::FirstSlotOrdinalToFish;
	bool direction = true;

	// Default value
	DeltaTime durationFishSession = FishingParameters::TrollingFishSessionDurationTicks;
}



// !!! This should be the same as above compile time initialization.
void SyncRecoveryTrollingPolicy::restart() {
	Logger::log("reset FishPolicy\n");

	upCounter = FishingParameters::FirstSlotOrdinalToFish;
	downCounter = FishingParameters::LastSlotOrdinalToFish;
	direction = true;
	// next generated ordinal will be first sleeping slot
}

SlotCount SyncRecoveryTrollingPolicy::nextFishSlotOrdinal() {
	SlotCount result;

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

LongTime SyncRecoveryTrollingPolicy::getStartTimeToFish() {
	SlotCount sleepingSlotOrdinal = nextFishSlotOrdinal();

	// minus 1: convert ordinal to zero-based duration multiplier
	LongTime result = clique.schedule.startTimeOfSyncPeriod() +  (sleepingSlotOrdinal - 1) * ScheduleParameters::VirtualSlotDuration;

	return result;
}




/*
 * Duration methods
 */

DeltaTime SyncRecoveryTrollingPolicy::getFishSessionDuration() {
	// Variable with constant default
	return durationFishSession;
}

void SyncRecoveryTrollingPolicy::incrementFishSessionDuration(unsigned int increment) {
	Logger::log("Inc fish duration.\n");
	// increment is in ticks
	// TODO limit it.  For now assume it never gets larger than all of sync period.
	durationFishSession += increment;
}

void SyncRecoveryTrollingPolicy::decrementFishSessionDuration(unsigned int decrement){
	Logger::log("Dec fish duration.\n");

	// Prevent unsigned subraction overflow (i.e. < zero)
	if (durationFishSession > decrement)
		durationFishSession -= decrement;

	// Prevent less than minimum required by other code
	if (durationFishSession < FishingParameters::TrollingFishSessionDurationTicks)
		durationFishSession = FishingParameters::TrollingFishSessionDurationTicks;
}


void SyncRecoveryTrollingPolicy::setDurationToMinDuration() {
	durationFishSession = FishingParameters::TrollingFishSessionDurationTicks;
}

