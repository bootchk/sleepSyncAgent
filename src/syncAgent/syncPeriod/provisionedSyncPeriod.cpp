#include <cassert>

#include "syncPeriod.h"

// implementation
#include "../state/phase.h"
#include "../slots/syncing/syncWorkSlot.h"
#include "../sleepers/scheduleSleeper.h"
#include "../logging/logger.h"
#include "../syncAgent.h"

#include <provisioner.h>


namespace {
SyncWorkSlot syncWorkSlot;

void provisioningFailedCallback() {
	Logger::log("\nprovision fail");
}


void provisioningSuccededCallback() {
	// TODO pass the provisioned value
	Logger::log("\nprovision succeed");
	// TODO do something with provisioned value
}

void tryProvision() {
	ScheduleSleeper::sleepUntilProvisionSlot();

	// Init on each session.  TODO init once.
	Provisioner::init(provisioningSuccededCallback, provisioningFailedCallback);

	// low-power blocks for duration of provisioning session
	bool result = Provisioner::provisionWithSleep();
	if (result)
		Logger::log("\n  WAS PROVISIONED");
	else
		Logger::log("\n  NOT PROVISIONED");

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
