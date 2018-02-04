#include <cassert>

#include "syncPeriod.h"

// implementation
#include "../state/phase.h"
#include "../slots/syncing/syncWorkSlot.h"
#include "../sleepers/scheduleSleeper.h"
#include "../logging/logger.h"
#include "../syncAgent.h"
#include "../scheduleParameters.h"

// libBLEProvisionee
#include <provisioner.h>




namespace {
SyncWorkSlot syncWorkSlot;

void provisioningFailedCallback() {
	Logger::log("\nprovision fail");
}


DeltaTime calculatePeriodTime(uint8_t offsetTime) {
	/*
	 * offsetTime is a numerator of fraction (having denominator 255) of 10 seconds
	 * Convert to ticks without loss of precsion i.e. multiply before divide
	 */

	unsigned int ticksBeforeNowOfButtonPush = (10 * offsetTime * ScheduleParameters::TicksPerSecond) / 255;

	/*
	 * Period time  when button was pushed (if periods have not changed recently.
	 */
	return PeriodTime::convertTickOffset(ticksBeforeNowOfButtonPush);
}



void provisioningSuccededCallback(
		uint8_t provisionedValue,
		int8_t rssi
		) {
	// TODO pass the provisioned index
	Logger::log("\nprovision succeed");

	// uint8_t value = Provisioner::getProvisionedValue();
	Logger::log(provisionedValue);
	Logger::log("\nrssi: ");
	Logger::log((uint8_t)rssi);

	// Observer pattern
	SyncAgent::notifyProvisionObservers(calculatePeriodTime(provisionedValue), rssi);
}


void tryProvision() {
	ScheduleSleeper::sleepUntilProvisionSlot();
	Logger::logTicksSinceStartSyncPeriod();
	// Init on each session.  TODO init once.
	Provisioner::init(provisioningSuccededCallback, provisioningFailedCallback);

	// blocks low-power for duration of provisioning session
	bool result = Provisioner::provisionWithSleep();

	Logger::logTicksSinceStartSyncPeriod();

	Logger::logResult("provisioned", result);

	/*
	 * Since Provisioner reconfigured radio, restore to SleepSync configuration.
	 */
	SyncAgent::initEnsembleProtocol();

	// TODO
	// assert a provisioning session is not longer than syncPeriod
	// assert we are not past end of syncPeriod
}




}	// namespace



void ProvisioningSyncPeriod::doSlotSequence() {

	// the caller, at schedule.rollPeriodForward has logged syncPoint

	// assert power is adequate

	Phase::set(PhaseEnum::StartSlotSequence);
	syncWorkSlot.tryPerform();	// arbitrary to do sync slot first

	assert(Ensemble::isLowPower());	// until next slot

	/*
	 * SyncSlot might have exhausted power.
	 * As we proceed, we check power again just before we need the power.
	 */
    tryProvision();

	assert(Ensemble::isLowPower());	// For remainder of sync period

	Phase::set(PhaseEnum::SleepRemainder);
	ScheduleSleeper::sleepRemainderOfSyncPeriod();

	// Sync period (slot sequence) completed
}
