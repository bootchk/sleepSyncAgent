
#include "provisionerCallback.h"

#include "../logging/logger.h"
#include "provisioningPublisher.h"
#include "../task/task.h"
#include "../schedule/syncSchedule.h"



namespace {

void scheduleProvisionEnd() {
	SyncSchedule::provisionEnd();
}

}


/*
 * These are called from the context of a Softdevice Handler.
 * That is, a BLE event occurred under auspices of Softdevice.
 * That context is an ISR but not our task. (SD is configured to call BLEProvisioner::appHandler i.e. observer in ISR context)
 * None of our tasks are scheduled.
 *
 * Cannot disable SD from observer context.
 * So schedule a task to shutdown the Provisioner (disable Softdevice, so no more events from it.)
 */

void ProvisionerCallback::succeed(ProvisionedValueType provision,
		int8_t rssi)
{
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


	// TODO move this to endProvisionTask, to keep ISR from SD shorter

	// provision.index was checked earlier, but convert to enum
	ProvisionablePropertyIndex index = ProvisioningPublisher::ppiFromRawPPI(provision.index);
	if ( index != ProvisionablePropertyIndex::Invalid) {
		ProvisioningPublisher::notify(index, provision, rssi);
	}

	scheduleProvisionEnd();
}


void ProvisionerCallback::fail() {
	Logger::log("\nprovision fail");

	// No publish to app, it only wants succeed

	scheduleProvisionEnd();
}
