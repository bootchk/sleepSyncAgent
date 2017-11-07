
#include "fishingManager.h"
#include "fishPolicy.h"

namespace {
	FishingMode mode = FishingMode::Trolling;
}




LongTime FishingManager::getStartTimeToFish(){
	// Delegate to current fishing mode

	LongTime result;

	switch(mode){
	case FishingMode::Trolling:
		result = SyncRecoveryTrollingPolicy::getStartTimeToFish();
		break;
	case FishingMode::DeepFishing:
		break;
	}

	return result;
}


void FishingManager::restartTrollingMode() {
	SyncRecoveryTrollingPolicy::restart();
}
