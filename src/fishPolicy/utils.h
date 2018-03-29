
#include "../fishPolicy/fishingParameters.h"
#include "../fishPolicy/slottedFishSession.h"



namespace Fishing {

void incrementCounterModuloSleepingSlots(SlotCount* counter);
void decrementCounterModuloSleepingSlots(SlotCount* counter);


/*
 * In this design, the up counter may skip past slots at end of SyncPeriod,
 * but down counter will fish them.
 * Thus slots near the SyncSlot are fished less often then others.
 */
void incrementCounterModuloSleepingSlots(SlotCount* counter) {
	(*counter) += SlottedFishSession::durationInSlots();

	// If overlaps following sync slot, skip past

	if (SyncRecoveryTrollingPolicy::currentSessionEndSlotOrdinal() > FishingParameters::LastSlotOrdinalToFish) {
			*counter = SyncRecoveryTrollingPolicy::getInitialUpCounter();
	}
	/*
	Trivial case where slotDuration is 1
	if (*counter > FishingParameters::LastSlotOrdinalToFish) {
		*counter = FishingParameters::FirstSlotOrdinalToFish;
	}
	*/
}

void decrementCounterModuloSleepingSlots(SlotCount* counter) {
	(*counter) -= SlottedFishSession::durationInSlots();
	if (SyncRecoveryTrollingPolicy::currentSessionStartSlotOrdinal() < FishingParameters::FirstSlotOrdinalToFish) {
		*counter = SyncRecoveryTrollingPolicy::getInitialDownCounter();
	}
	/*
	 if (*counter < FishingParameters::FirstSlotOrdinalToFish) {
		*counter = FishingParameters::LastSlotOrdinalToFish;
	}
	*/
}
}
