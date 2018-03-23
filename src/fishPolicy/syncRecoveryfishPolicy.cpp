
#include "fishPolicy.h"
#include "utils.h" // increment/decrement

#include "slottedFishSession.h"

#include "../clique/clique.h"
#include "../globals.h"   // clique
#include "../logging/logger.h"





using namespace Fishing;


namespace {

/*
 * !!! upCounter initialized to  last slot, the first call after reset increments,
 * and the first result is FirstSleepingSlotOrdinal.
 */
	SlotCount upCounter = FishingParameters::LastSlotOrdinalToFish;
	SlotCount downCounter = FishingParameters::FirstSlotOrdinalToFish;
	bool direction = true;
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
	return SlottedFishSession::duration();
}


