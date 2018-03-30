
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
	SlotCount downCounter = FishingParameters::LastSlotOrdinalShouldFish;

	/* Default is start and end on same slot. */
	SlotCount currentStartSlotOrdinal = FishingParameters::FirstSlotOrdinalToFish;
	SlotCount currentEndSlotOrdinal = FishingParameters::FirstSlotOrdinalToFish;
	bool direction = true;


bool isInRange() {
	return currentStartSlotOrdinal >= FishingParameters::FirstSlotOrdinalToFish
				and currentEndSlotOrdinal <= FishingParameters::LastSlotOrdinalShouldFish;
}


void setSessionForStartSlot( SlotCount startSlot) {
	currentStartSlotOrdinal = startSlot;
	currentEndSlotOrdinal = currentStartSlotOrdinal + SlottedFishSession::durationInSlots() - 1;
	assert(isInRange());
	Logger::log("Fish:"); Logger::logInt(currentStartSlotOrdinal); Logger::log(","); Logger::logInt(currentEndSlotOrdinal);
}


} // namespace


SlotCount SyncRecoveryTrollingPolicy::getInitialUpCounter() { return FishingParameters::FirstSlotOrdinalToFish; }
SlotCount SyncRecoveryTrollingPolicy::getInitialDownCounter() { return FishingParameters::LastSlotOrdinalShouldFish - SlottedFishSession::durationInSlots() + 1; }



// !!! This should be the same as above compile time initialization.
/*
 * Restart is called after a sync slot finds that sync was not received for a long time.
 * The next trolling fishing will be first fishing slot (abut sync slot.)
 */
void SyncRecoveryTrollingPolicy::restart() {
	Logger::log("reset FishPolicy\n");

	upCounter = getInitialUpCounter();
	downCounter =  getInitialDownCounter();
	Logger::log("Up:"); Logger::logInt(upCounter);
	Logger::log("Down:"); Logger::logInt(downCounter);

	setSessionForStartSlot(upCounter);
	direction = true;
}



SlotCount SyncRecoveryTrollingPolicy::currentSessionStartSlotOrdinal() { return currentStartSlotOrdinal; }
SlotCount SyncRecoveryTrollingPolicy::currentSessionEndSlotOrdinal() { return currentEndSlotOrdinal; }


// TODO if session duration is changed before starting, the above values are not right


bool SyncRecoveryTrollingPolicy::checkDone() {
	return false;	// Never done trolling
}

void SyncRecoveryTrollingPolicy::preFishing() {
	proceedToNextFishSlotOrdinal();
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

	direction = ! direction;	// alternate direction, i.e. counter to return next call

	setSessionForStartSlot(next);
}


LongTime SyncRecoveryTrollingPolicy::getStartTimeToFish() {

	// minus 1: convert ordinal to zero-based duration multiplier
	LongTime result = clique.schedule.startTimeOfSyncPeriod() +  (currentStartSlotOrdinal - 1) * ScheduleParameters::VirtualSlotDuration;

	return result;
}




// delegate to fishSession
DeltaTime SyncRecoveryTrollingPolicy::getFishSessionDurationTicks() {
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
	return currentStartSlotOrdinal == FishingParameters::FirstSlotOrdinalToFish;
}

bool SyncRecoveryTrollingPolicy:: isAbutFirstSleepingSlot() {
	return currentStartSlotOrdinal == FishingParameters::FirstSlotOrdinalToFish + 1;
}

bool SyncRecoveryTrollingPolicy::isCoverLastSleepingSlot() {
	return currentSessionEndSlotOrdinal() >=  FishingParameters::LastSlotOrdinalShouldFish ;
}

bool SyncRecoveryTrollingPolicy::isAbutLastSleepingSlot() {
	return currentSessionEndSlotOrdinal() == FishingParameters::LastSlotOrdinalShouldFish - 1;
}
