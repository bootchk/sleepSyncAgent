
#include "slottedFishSession.h"

#include "fishingParameters.h"

#include "../logging/logger.h"

#include "../fishPolicy/fishPolicy.h"


// TODO, parameters should be in slots only, and dynamic calculated from VirtualSlotDuration

namespace {
    // Default values

	// The design is: change these together

	// !!! Not old design "real" but "virtual" slots.  TODO, when abandon old design, discard these comments
	DeltaTime _dynamicFishSessionDurationInTicks = FishingParameters::MinTrollingFishSessionDurationTicks;
	SlotCount _sessionDurationInSlots = 1;	// TODO symbolic FishingParameters::MinTrollingFishSessionDurationSlots
}


DeltaTime SlottedFishSession::durationInTicks() {
	return _dynamicFishSessionDurationInTicks;
}

SlotCount SlottedFishSession::durationInSlots() {
	return _sessionDurationInSlots;
}



void SlottedFishSession::incrementFishSessionDuration(SlotCount increment) {

	Logger::logIncreaseFish();  Logger::logInt(increment);

	DeltaTime trialResult = _sessionDurationInSlots + increment;

	// Not larger than all of sync period.
	if (trialResult > FishingParameters::MaxTrollingRealFishSessionDurationTicks) {
		Logger::log("!!!Try overflow fish duration\n");
		return;
	}
	else {
		_sessionDurationInSlots = trialResult;
		_dynamicFishSessionDurationInTicks = trialResult * FishingParameters::MinTrollingFishSessionDurationTicks;
	}

	// Responsibility: Reset fish policy when parameters change
	SyncRecoveryTrollingPolicy::restart();
}

void SlottedFishSession::decrementFishSessionDuration(SlotCount decrement){
	// TEMP
	return;

	Logger::logDecreaseFish();

	// Prevent unsigned subraction overflow (i.e. < zero)
	if (_dynamicFishSessionDurationInTicks > decrement)
		_dynamicFishSessionDurationInTicks -= decrement;

	// Prevent less than minimum required by other code
	if (_dynamicFishSessionDurationInTicks < FishingParameters::MinTrollingRealFishSessionDurationTicks)
		_dynamicFishSessionDurationInTicks = FishingParameters::MinTrollingRealFishSessionDurationTicks;
}


void SlottedFishSession::setDurationToMinDuration() {
	Logger::logToMinFish();
	_sessionDurationInSlots = 1;
	_dynamicFishSessionDurationInTicks = FishingParameters::MinTrollingRealFishSessionDurationTicks;

	// TODO SyncRecoveryTrollingPolicy::restart();
}
