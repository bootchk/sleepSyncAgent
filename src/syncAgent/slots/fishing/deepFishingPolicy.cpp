
#include "fishPolicy.h"
#include "fishingParameters.h"



namespace {
	unsigned int countFishings;
	FishPolicyDoneCallback fishPolicyDoneCallback = nullptr;
}



void DeepFishingPolicy::restart() {
	countFishings = 0;
}


LongTime DeepFishingPolicy::getStartTimeToFish() {
	countFishings++;

	/*
	 * Start time is an offset from start of self sync period to start of sync period of other.
	 *
	 * Might not be aligned with slot boundaries.
	 *
	 * Ensure it is in the sync period.
	 * Ensure it is not the sync slot.
	 */
	// TODO return
}


void DeepFishingPolicy::setCallbackOnDone(FishPolicyDoneCallback) {

}


void DeepFishingPolicy::checkDone() {
	if (countFishings > FishingParameters::CountFishingsPerDeepFishing) {
		// never caught anything
		assert(fishPolicyDoneCallback != nullptr);
		fishPolicyDoneCallback();
	}
}
