
#include "provisionerCallback.h"

#include "../logging/logger.h"
#include "provisioningPublisher.h"
#include "../task/task.h"






/*
 * These are called from the context of a Softdevice Handler.
 * That is, a BLE event occurred under auspices of Softdevice.
 * That context is NOT an ISR i.e. not a task.
 * And none of our tasks are schedule.
 * Here, we shutdown the Provisioner (deactivate Softdevice, so no more events from it.)
 * And schedule a task.
 */

void ProvisionerCallback::succeed(ProvisionedValueType provision,
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

	SSTask::provisionEnd();
}


void ProvisionerCallback::fail() {
	Logger::log("\nprovision fail");

	// No publish to app, it only wants succeed

	SSTask::provisionEnd();
}
