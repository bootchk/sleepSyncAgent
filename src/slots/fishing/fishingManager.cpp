
#include "fishingManager.h"
#include "../../logging/logger.h"
#include "../../fishPolicy/fishPolicy.h"



namespace {
	FishingMode fishingMode = FishingMode::Trolling;
}


FishingMode FishingManager::mode() { return fishingMode; }


/*
 * Called many times (for repetitive Merge class msgs), but only restarts on the first.
 *
 * Repetitive Merge class msgs might have different parameters, different happenings.
 * Overlapping happenings should complete one DeepFishing and then start another?
 * For now, not checking for interleaved different msgs.
 */
// XXX Check , and chose high priority?
void FishingManager::switchToDeepFishing(DeltaTime deltaToSyncPointOfFish, Callback aCallback) {
	if (fishingMode != FishingMode::DeepFishing) {
		Logger::log("\nto deep ");
		fishingMode = FishingMode::DeepFishing;
		DeepFishingPolicy::restart(deltaToSyncPointOfFish, aCallback);
	}
}

void FishingManager::switchToTrolling() {
	Logger::log("\nto troll ");
	fishingMode = FishingMode::Trolling;
}

void FishingManager::restartTrollingMode() {
	// Not require mode is Trolling
	SyncRecoveryTrollingPolicy::restart();
}




void FishingManager::checkFishingDone() {
	// Delegate to current fishing mode
	switch(fishingMode){
	case FishingMode::Trolling:
		// It never returns true, don't care
		(void) SyncRecoveryTrollingPolicy::checkDone();
		break;
	case FishingMode::DeepFishing:
		if (DeepFishingPolicy::checkDone()) {
			// side effect of checkDone() was call callback.  See slot/fishing/fishingDoneCallbacks
			switchToTrolling();
		}
		break;
	}
}


LongTime FishingManager::getStartTimeToFish(){

	LongTime result;

	switch(fishingMode){
	case FishingMode::Trolling:
	default:
		result = SyncRecoveryTrollingPolicy::getStartTimeToFish();
		break;
	case FishingMode::DeepFishing:
		result = DeepFishingPolicy::getStartTimeToFish();
		break;
	}
	/*
	 * Not ensure is in future nor before end of sync period.
	 */
	return result;
}

DeltaTime FishingManager::getFishSessionDuration(){

	DeltaTime result;

	switch(fishingMode){
	case FishingMode::Trolling:
	default:
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



bool FishingManager::isFishSlotStartSyncPeriod() {
	bool result;

	switch(fishingMode){
	case FishingMode::Trolling:
	default:
		result = SyncRecoveryTrollingPolicy::isFishSessionNearStartSyncPeriod();
		break;
	case FishingMode::DeepFishing:
		result = DeepFishingPolicy::isFishSlotStartSyncPeriod();
		break;
	}
	return result;
}

bool FishingManager::isFishSlotEndSyncPeriod() {
bool result;

	switch(fishingMode){
	case FishingMode::Trolling:
	default:
		result = SyncRecoveryTrollingPolicy::isFishSessionNearEndSyncPeriod();
		break;
	case FishingMode::DeepFishing:
		result = DeepFishingPolicy::isFishSlotEndSyncPeriod();
		break;
	}
	return result;
}
