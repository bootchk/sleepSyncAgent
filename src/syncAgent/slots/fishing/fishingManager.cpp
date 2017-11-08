
#include "fishingManager.h"
#include "fishPolicy.h"

namespace {
	FishingMode mode = FishingMode::Trolling;
}

void FishingManager::switchToDeepFishing() {
	mode = FishingMode::DeepFishing;
	DeepFishingPolicy::restart();
}

void FishingManager::switchToTrolling() {
	mode = FishingMode::Trolling;
}

void FishingManager::restartTrollingMode() {
	SyncRecoveryTrollingPolicy::restart();
}




void FishingManager::checkFishingDone() {
	// Delegate to current fishing mode
	switch(mode){
	case FishingMode::Trolling:
		SyncRecoveryTrollingPolicy::checkDone();
		break;
	case FishingMode::DeepFishing:
		DeepFishingPolicy::checkDone();
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



