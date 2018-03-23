

#ifdef NOT_USED

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


#endif
