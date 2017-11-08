
#include "fishingManager.h"
#include "fishPolicy.h"

namespace {
	FishingMode mode = FishingMode::Trolling;
}

void FishingManager::switchToDeepFishing(DeltaTime deltaToSyncPointOfFish, Callback aCallback) {
	mode = FishingMode::DeepFishing;
	DeepFishingPolicy::restart(deltaToSyncPointOfFish, aCallback);
}

void FishingManager::switchToTrolling() {
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
	return result;
}

