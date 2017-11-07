
#include "fishingParameters.h"

namespace Fishing {

void incrementCounterModuloSleepingSlots(ScheduleCount* counter);
void decrementCounterModuloSleepingSlots(ScheduleCount* counter);

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
