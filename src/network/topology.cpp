#include <radioSoC.h>
#include "topology.h"

#include "intraCliqueManager.h"

#include "../syncAgent/syncAgent.h"
#include "../syncAgent/provisioningPublisher.h"


void NetworkTopology::subscribeToProvisionings() {
	ProvisioningPublisher::subscribe(2, NetworkTopology::handleCliqueSizeProvisioning);
	ProvisioningPublisher::subscribe(3, NetworkTopology::handleScatterProvisioning);
}

/*
 * Current design: we don't care if it is from master or slave
 * but we do care that it is from our clique.
 */
void NetworkTopology::setGranularity(SyncMessage* msg) {

	// WAS void NetworkTopology::setXmitPower(WorkPayload xmitPowerValue) {
	// Requires radio not in use, required by Radio::

	WorkPayload xmitPowerValue = msg->work;

	// Check validity of OTA value
	if (Radio::isValidXmitPower(static_cast<TransmitPowerdBm>(xmitPowerValue))) {
		Radio::configureXmitPower(static_cast<TransmitPowerdBm>(xmitPowerValue));
	}

	// Ensure xmit power set, ensured by Radio::
}




/*
 * Slaves do not set control clique directly,
 * but relay to Master and wait till Master tells them.
 */


void NetworkTopology::handleCliqueSizeProvisioning(uint32_t provisionedValue) {
	// cast raw OTA provisionedValue
	NetGranularity granularity = static_cast<NetGranularity>(provisionedValue);

	if (SyncAgent::isSelfMaster())
		IntraCliqueManager::doDownstreamCliqueSizeChange(granularity);
	else
		IntraCliqueManager::doUpstreamCliqueSizeChange(granularity);
}

void NetworkTopology::handleScatterProvisioning(uint32_t  provisionedValue) {
	/*
	 * provisionedValue is a signal: value not used.
	 */
	(void) provisionedValue;

	if (SyncAgent::isSelfMaster())
		IntraCliqueManager::doDownstreamScatter();
	else
		IntraCliqueManager::doUpstreamScatter();
}
