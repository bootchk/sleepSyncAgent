
#include "fishPolicy.h"
#include "utils.h" // increment/decrement

#include "slottedFishSession.h"

#include "../clique/clique.h"
#include "../globals.h"   // clique

#include "../logging/logger.h"


using namespace Fishing;	// utils.h


// TODO, should be an init function and not duplicate code below

namespace {

/*
 * In the old design, we advanced just before fishing.
 * In new design, we advance after fishing.
 *
 * In old design upCounter initialized to  last slot, the first call after reset increments,
 * and the first result is FirstSleepingSlotOrdinal.
 */
	SlotCount upCounter = FishingParameters::FirstSlotOrdinalToFish ;
	SlotCount downCounter = FishingParameters::LastSlotOrdinalToFish;
	SlotCount currentSlotOrdinal = FishingParameters::FirstSlotOrdinalToFish;
	bool direction = true;
}

// !!! This should be the same as above compile time initialization.
/*
 * Restart is called after a sync slot finds that sync was not received for a long time.
 * The next trolling fishing will be first fishing slot (abut sync slot.)
 */
void SyncRecoveryTrollingPolicy::restart() {
	Logger::log("reset FishPolicy\n");

	upCounter = FishingParameters::FirstSlotOrdinalToFish;
	downCounter = FishingParameters::LastSlotOrdinalToFish;
	currentSlotOrdinal = FishingParameters::FirstSlotOrdinalToFish;
	direction = true;
}





SlotCount SyncRecoveryTrollingPolicy::currentSessionStartSlotOrdinal() {
	assert(currentSlotOrdinal >= FishingParameters::FirstSlotOrdinalToFish
			and currentSlotOrdinal <= FishingParameters::LastSlotOrdinalToFish);
	return currentSlotOrdinal;
}


// TODO if session duration is changed before starting, the above values are not right


bool SyncRecoveryTrollingPolicy::checkDone() {
	/*
	 * A trolling fish session has been done.  Move to next one.
	 */
	proceedToNextFishSlotOrdinal();
	return false;	// Never done trolling
}


/*
 * This must only be called once per sync period.
 */
void SyncRecoveryTrollingPolicy::proceedToNextFishSlotOrdinal() {
	SlotCount next;

	if (direction) {
		incrementCounterModuloSleepingSlots(&upCounter);
		next = upCounter;
	}
	else {
		decrementCounterModuloSleepingSlots(&downCounter);
		next = downCounter;
	}

	direction = ! direction;	// reverse direction, i.e. counter to return next call

	currentSlotOrdinal = next;	// Validity checked on retrieval
}


LongTime SyncRecoveryTrollingPolicy::getStartTimeToFish() {

	// minus 1: convert ordinal to zero-based duration multiplier
	LongTime result = clique.schedule.startTimeOfSyncPeriod() +  (currentSlotOrdinal - 1) * ScheduleParameters::VirtualSlotDuration;

	return result;
}




// delegate to fishSession
DeltaTime SyncRecoveryTrollingPolicy::getFishSessionDuration() {
	// Variable with constant default
	return SlottedFishSession::durationInTicks();
}



bool SyncRecoveryTrollingPolicy::isFishSessionNearStartSyncPeriod() {
	return isCoverFirstSleepingSlot() or isAbutFirstSleepingSlot();
}

bool SyncRecoveryTrollingPolicy::isFishSessionNearEndSyncPeriod() {
	return isCoverLastSleepingSlot() or isAbutLastSleepingSlot();
}


// Private

bool SyncRecoveryTrollingPolicy::isCoverFirstSleepingSlot() {
	return currentSlotOrdinal == FishingParameters::FirstSlotOrdinalToFish;
}

bool SyncRecoveryTrollingPolicy:: isAbutFirstSleepingSlot() {
	return currentSlotOrdinal == FishingParameters::FirstSlotOrdinalToFish + 1;
}

bool SyncRecoveryTrollingPolicy::isCoverLastSleepingSlot() {
	return SlottedFishSession::lastSlotOrdinal() >=  FishingParameters::LastSlotOrdinalToFish ;
}

bool SyncRecoveryTrollingPolicy::isAbutLastSleepingSlot() {
	return SlottedFishSession::lastSlotOrdinal() == FishingParameters::LastSlotOrdinalToFish - 1;
}
