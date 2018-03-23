
#include "slottedFishSession.h"

#include "fishingParameters.h"


#include "../logging/logger.h"

namespace {
    // Default value
	DeltaTime dynamicFishSessionDuration = FishingParameters::MinTrollingRealFishSessionDurationTicks;
}


DeltaTime SlottedFishSession::duration() {
	return dynamicFishSessionDuration;
}


void SlottedFishSession::incrementFishSessionDuration(unsigned int increment) {
	// TEMP, until I revise this
	// the problem is that I need to offset the startTime by the duration
	// so that I don't log late end time of fishing
	return;

	Logger::logIncreaseFish();  Logger::logInt(increment);

	DeltaTime trialResult = dynamicFishSessionDuration += increment;

	// Not larger than all of sync period.
	if (trialResult > FishingParameters::MaxTrollingRealFishSessionDurationTicks) {
		Logger::log("!!!Try overflow fish duration\n");
		return;
	}
	else
		dynamicFishSessionDuration = trialResult;
}

void SlottedFishSession::decrementFishSessionDuration(unsigned int decrement){
	Logger::logDecreaseFish();

	// Prevent unsigned subraction overflow (i.e. < zero)
	if (dynamicFishSessionDuration > decrement)
		dynamicFishSessionDuration -= decrement;

	// Prevent less than minimum required by other code
	if (dynamicFishSessionDuration < FishingParameters::MinTrollingRealFishSessionDurationTicks)
		dynamicFishSessionDuration = FishingParameters::MinTrollingRealFishSessionDurationTicks;
}


void SlottedFishSession::setDurationToMinDuration() {
	Logger::logToMinFish();
	dynamicFishSessionDuration = FishingParameters::MinTrollingRealFishSessionDurationTicks;
}
