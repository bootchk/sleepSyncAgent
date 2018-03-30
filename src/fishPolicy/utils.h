
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

/*
 * Implementation notes:  counter is unsigned, take care not to subtract possibly below zero
 */
void incrementCounterModuloSleepingSlots(SlotCount* counter) {
	SlotCount result;

	result = *counter + SlottedFishSession::durationInSlots();

	// If overlaps (last slot of session beyond last slot to fish) following sync slot, skip past

	if (result + SlottedFishSession::durationInSlots() > FishingParameters::LastSlotOrdinalShouldFish) {
			result = SyncRecoveryTrollingPolicy::getInitialUpCounter();
	}
	(*counter) =result;
	/*
	Trivial case where slotDuration is 1
	if (*counter > FishingParameters::LastSlotOrdinalShouldFish) {
		*counter = FishingParameters::FirstSlotOrdinalToFish;
	}
	*/
}

void decrementCounterModuloSleepingSlots(SlotCount* counter) {
	SlotCount result;

	if ( *counter < FishingParameters::FirstSlotOrdinalToFish + SlottedFishSession::durationInSlots()) {
		/*
		 * Subtraction would underflow, i.e. would overlap preceding sync slot.
		 * Circularly skip past the sync slot.
		 */
		result = SyncRecoveryTrollingPolicy::getInitialDownCounter();
	}
	else {
		result = *counter - SlottedFishSession::durationInSlots();
	}
	*counter =result;

	/*
	 if (*counter < FishingParameters::FirstSlotOrdinalToFish) {
	 *counter = FishingParameters::LastSlotOrdinalShouldFish;
	}
	 */
}

}	// namespace
