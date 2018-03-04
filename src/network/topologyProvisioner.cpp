
#include "topologyProvisioner.h"

#include "../provisioning/provisioningPublisher.h"
#include "topology.h"

#include "../syncAgentImp/syncAgentImp.h"
#include "intraCliqueManager.h"

#include "../logging/logger.h"




void NetTopologyProvisioner::subscribeToProvisionings() {
	ProvisioningPublisher::subscribe(
			ProvisionablePropertyIndex::NetGranularity,
			NetTopologyProvisioner::handleNetGranularityProvisioning);
	ProvisioningPublisher::subscribe(
			ProvisionablePropertyIndex::Scatter,
			NetTopologyProvisioner::handleScatterProvisioning);
}


/*
 * Slaves do not set control clique directly,
 * but relay to Master and wait till Master tells them.
 */


void NetTopologyProvisioner::handleNetGranularityProvisioning(uint32_t provisionedValue) {

	NetGranularity granularity = Granularity::getFromRaw(static_cast<unsigned char> (provisionedValue));
	if (granularity == NetGranularity::Invalid) {
		Logger::log("Provisioned granularity invalid\n");
		return;
	}

	if (SyncAgentImp::isSelfMaster()) {
		NetworkTopology::masterTellCliqueGranularityChange(granularity);
	}
	else {
		// We are slave, relay upstream to master.
		// Will go OTA and could get garbled again.
		IntraCliqueManager::doUpstreamCliqueSizeChange(granularity);
	}

}

void NetTopologyProvisioner::handleScatterProvisioning(uint32_t  provisionedValue) {
	/*
	 * provisionedValue is a signal: value not used.
	 */
	(void) provisionedValue;

	if (SyncAgentImp::isSelfMaster()) {
		// same as above, don't scatter self until slaves told
		IntraCliqueManager::doDownstreamScatter();
	}
	else
		IntraCliqueManager::doUpstreamScatter();
}

