
#include "fishPolicy.h"
#include "fishingParameters.h"

#include "../../clique/clique.h"
#include "../../globals.h"   // clique

#include "../../logging/logger.h"


namespace {
	unsigned int countFishings;

	/*
	 * Info received in a MasterMergedAway or SlaveMergedAway msg in my SyncSlot
	 * from the Master or Slave who merged away, who is now in the Merger role.
	 */
	DeltaTime deltaSyncPointToSyncPointOfFishee;

	Callback fishPolicyDoneCallback = nullptr;
}



void DeepFishingPolicy::restart(DeltaTime aDeltaToSyncPointOfFish, Callback aCallback) {
	countFishings = 0;
	fishPolicyDoneCallback = aCallback;
	deltaSyncPointToSyncPointOfFishee = aDeltaToSyncPointOfFish;
}


LongTime DeepFishingPolicy::getStartTimeToFish() {
	countFishings++;

	/*
	 * Start time of time to start fishing.
	 * Should be aligned with start of sync period of other.
	 * Might not be aligned with slot boundaries of this clique.
	 *
	 * We saved an offset from start of sync period to time to fish earlier.
	 */
	LongTime result;
	result = clique.schedule.startTimeOfSyncPeriod() + deltaSyncPointToSyncPointOfFishee;

	/*
	 * Is called at end of SyncSlot.
	 *
	 * Not ensure in future.
	 * Not ensure not too near end of sync period.
	 * Caller must not use result to schedule in the past or beyond sync period.
	 */

	return result;
}




bool DeepFishingPolicy::checkDone() {
	bool result = false;
	if (countFishings > FishingParameters::CountFishingsPerDeepFishing) {
		// never caught anything
		Logger::log("\ndeep over");

		// Generate event
		assert(fishPolicyDoneCallback != nullptr);
		fishPolicyDoneCallback();

		result = true;
	}
	return result;
}
