
#include "fishPolicy.h"
#include "fishingParameters.h"

#include "../../clique/clique.h"
#include "../../globals.h"   // clique

namespace {
	unsigned int countFishings;
	DeltaTime deltaToSyncPointOfFish;
	Callback fishPolicyDoneCallback = nullptr;
}



void DeepFishingPolicy::restart(DeltaTime aDeltaToSyncPointOfFish, Callback aCallback) {
	countFishings = 0;
	fishPolicyDoneCallback = aCallback;
	deltaToSyncPointOfFish = aDeltaToSyncPointOfFish;
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
	LongTime result;
	result = clique.schedule.startTimeOfSyncPeriod() + deltaToSyncPointOfFish;

	/*
	 * Is called at end of SyncSlot.
	 * Ensure in future.
	 */
	//TODO assert(result > clique.schedule.nowTime());

	return result;
}




bool DeepFishingPolicy::checkDone() {
	bool result = false;
	if (countFishings > FishingParameters::CountFishingsPerDeepFishing) {
		// never caught anything

		// Generate event
		assert(fishPolicyDoneCallback != nullptr);
		fishPolicyDoneCallback();

		result = true;
	}
	return result;
}
