
#include "slottedFishSession.h"

#include "fishingParameters.h"

#include "../logging/logger.h"

#include "../fishPolicy/fishPolicy.h"




namespace {
    // Default values

	// The design is: change these together
	DeltaTime _dynamicFishSessionDuration = FishingParameters::MinTrollingRealFishSessionDurationTicks;
	SlotCount _sessionDurationInSlots = 1;
}


DeltaTime SlottedFishSession::durationInTicks() {
	return _dynamicFishSessionDuration;
}

SlotCount SlottedFishSession::durationInSlots() {
	return _sessionDurationInSlots;
}

SlotCount SlottedFishSession::lastSlotOrdinal() {
	return SyncRecoveryTrollingPolicy::currentSessionStartSlotOrdinal() + durationInSlots();
}

// TODO increment in slots * slotDuration
void SlottedFishSession::incrementFishSessionDuration(SlotCount increment) {
	// TEMP, until I revise this
	// the problem is that I need to offset the startTime by the duration
	// so that I don't log late end time of fishing
	return;

	Logger::logIncreaseFish();  Logger::logInt(increment);

	DeltaTime trialResult = _dynamicFishSessionDuration += increment;

	// Not larger than all of sync period.
	if (trialResult > FishingParameters::MaxTrollingRealFishSessionDurationTicks) {
		Logger::log("!!!Try overflow fish duration\n");
		return;
	}
	else
		_dynamicFishSessionDuration = trialResult;
}

void SlottedFishSession::decrementFishSessionDuration(SlotCount decrement){
	// TEMP
	return;

	Logger::logDecreaseFish();

	// Prevent unsigned subraction overflow (i.e. < zero)
	if (_dynamicFishSessionDuration > decrement)
		_dynamicFishSessionDuration -= decrement;

	// Prevent less than minimum required by other code
	if (_dynamicFishSessionDuration < FishingParameters::MinTrollingRealFishSessionDurationTicks)
		_dynamicFishSessionDuration = FishingParameters::MinTrollingRealFishSessionDurationTicks;
}


void SlottedFishSession::setDurationToMinDuration() {
	Logger::logToMinFish();
	_sessionDurationInSlots = 1;
	_dynamicFishSessionDuration = FishingParameters::MinTrollingRealFishSessionDurationTicks;
}
