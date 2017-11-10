
#include "fishingManager.h"
#include "fishPolicy.h"

#include "../../logging/logger.h"



namespace {
	FishingMode mode = FishingMode::Trolling;
}

/*
 * Called many times (for repetitive Merge class msgs), but only restarts on the first.
 *
 * Repetitive Merge class msgs might have different parameters, different happenings.
 * Overlapping happenings should complete one DeepFishing and then start another?
 * For now, not checking for interleaved different msgs.
 */
// XXX Check , and chose high priority?
void FishingManager::switchToDeepFishing(DeltaTime deltaToSyncPointOfFish, Callback aCallback) {
	if (mode != FishingMode::DeepFishing) {
		Logger::log("\nto deep ");
		mode = FishingMode::DeepFishing;
		DeepFishingPolicy::restart(deltaToSyncPointOfFish, aCallback);
	}
}

void FishingManager::switchToTrolling() {
	Logger::log("\nto troll ");
	mode = FishingMode::Trolling;
}

void FishingManager::restartTrollingMode() {
	// Not require mode is Trolling
	SyncRecoveryTrollingPolicy::restart();
}




void FishingManager::checkFishingDone() {
	// Delegate to current fishing mode
	switch(mode){
	case FishingMode::Trolling:
		// It never returns true, don't care
		(void) SyncRecoveryTrollingPolicy::checkDone();
		break;
	case FishingMode::DeepFishing:
		if (DeepFishingPolicy::checkDone()) {
			switchToTrolling();
		}
		break;
	}
}


LongTime FishingManager::getStartTimeToFish(){

	LongTime result;

	switch(mode){
	case FishingMode::Trolling:
		result = SyncRecoveryTrollingPolicy::getStartTimeToFish();
		break;
	case FishingMode::DeepFishing:
		result = DeepFishingPolicy::getStartTimeToFish();
		break;
	}
	/*
	 * Not ensure is in future nor beyond end of sync period.
	 */
	return result;
}

DeltaTime FishingManager::getFishSessionDuration(){

	DeltaTime result;

	switch(mode){
	case FishingMode::Trolling:
		result = SyncRecoveryTrollingPolicy::getFishSessionDuration();
		break;
	case FishingMode::DeepFishing:
		result = DeepFishingPolicy::getFishSessionDuration();
		break;
	}
	/*
	 * Not ensure is in future nor beyond end of sync period.
	 */
	return result;
}

