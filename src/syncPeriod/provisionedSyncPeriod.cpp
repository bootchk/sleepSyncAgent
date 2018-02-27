#include <cassert>

#include "syncPeriod.h"

// implementation
#include "../slots/syncing/syncWorkSlot.h"
#include "../sleepers/scheduleSleeper.h"
#include "../logging/logger.h"

#include "../provisioning/provisioningPublisher.h"

// libBLEProvisionee
#include <provisioner.h>
#include "../syncAgentImp/state/phase.h"
#include "../syncAgentImp/syncAgentImp.h"




namespace {
SyncWorkSlot syncWorkSlot;

void provisioningFailedCallback() {
	Logger::log("\nprovision fail");
}


void provisioningSuccededCallback(
		ProvisionedValueType provision,
		int8_t rssi
		) {
	Logger::log("\nprovision succeed");

	// uint8_t value = Provisioner::getProvisionedValue();
	Logger::log("\nindex: ");
	Logger::log(provision.index);
	Logger::log("\nvalue: ");
	Logger::log(provision.value);
	Logger::log("\noffset: ");
	Logger::log(provision.offset);
	Logger::log("\nrssi: ");
	Logger::log((uint8_t)rssi);

	// provision.index was checked earlier, but convert to enum
	ProvisionablePropertyIndex index = ProvisioningPublisher::ppiFromRawPPI(provision.index);
	if ( index != ProvisionablePropertyIndex::Invalid) {
		ProvisioningPublisher::notify(index, provision, rssi);
	}

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
	SyncAgentImp::initEnsembleProtocol();

	// XXX
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
