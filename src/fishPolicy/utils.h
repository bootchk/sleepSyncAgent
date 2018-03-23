
#include "../fishPolicy/fishingParameters.h"

namespace Fishing {

void incrementCounterModuloSleepingSlots(SlotCount* counter);
void decrementCounterModuloSleepingSlots(SlotCount* counter);

void incrementCounterModuloSleepingSlots(SlotCount* counter) {
	(*counter)++;
	if (*counter > FishingParameters::LastSlotOrdinalToFish) {
		*counter = FishingParameters::FirstSlotOrdinalToFish;
	}
}

void decrementCounterModuloSleepingSlots(SlotCount* counter) {
	(*counter)--;
	if (*counter < FishingParameters::FirstSlotOrdinalToFish) {
		*counter = FishingParameters::LastSlotOrdinalToFish;
	}
}
}
