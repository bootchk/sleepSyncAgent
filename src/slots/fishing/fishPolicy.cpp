
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
	DeltaTime dynamicFishSessionDuration = FishingParameters::MinTrollingRealFishSessionDurationTicks;
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

	Logger::log(" Troll ");  Logger::logInt(result);

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
 *
 * inc/dec unit is ticks
 * Fishing need not be in is units of slots.
 */

DeltaTime SyncRecoveryTrollingPolicy::getFishSessionDuration() {
	// Variable with constant default
	return dynamicFishSessionDuration;
}

void SyncRecoveryTrollingPolicy::incrementFishSessionDuration(unsigned int increment) {
	// TEMP, until I revise this
	// the problem is that I need to offset the startTime by the duration
	// so that I don't log late end time of fishing
	return;

	Logger::logIncreaseFish();  Logger::logInt(increment);

	DeltaTime trialResult = dynamicFishSessionDuration += increment;

	// Not larger than all of sync period.
	if (trialResult > FishingParameters::MaxTrollingRealFishSessionDurationTicks) {
		Logger::log("!!!Try overflow fish duration\n");
		return;
	}
	else
		dynamicFishSessionDuration = trialResult;
}

void SyncRecoveryTrollingPolicy::decrementFishSessionDuration(unsigned int decrement){
	Logger::logDecreaseFish();

	// Prevent unsigned subraction overflow (i.e. < zero)
	if (dynamicFishSessionDuration > decrement)
		dynamicFishSessionDuration -= decrement;

	// Prevent less than minimum required by other code
	if (dynamicFishSessionDuration < FishingParameters::MinTrollingRealFishSessionDurationTicks)
		dynamicFishSessionDuration = FishingParameters::MinTrollingRealFishSessionDurationTicks;
}


void SyncRecoveryTrollingPolicy::setDurationToMinDuration() {
	Logger::logToMinFish();
	dynamicFishSessionDuration = FishingParameters::MinTrollingRealFishSessionDurationTicks;
}

