
#include "topology.h"

#include "granularity.h"

#include "../globals.h"  // clique
#include "intraCliqueManager.h"

#include "../syncAgent/syncAgent.h"
#include "../syncAgent/provisioningPublisher.h"

#include "../clique/clique.h"




namespace {

void masterTellCliqueMembers(NetGranularity granularity) {

	IntraCliqueManager::doDownstreamCliqueSizeChange(granularity);

	// TODO
	// Master set its own granularity later
	/*
	 * !!! We keep our current xmit power while we tell slaves,
	 * and then reduce our own xmit power.
	 * Spoofing.
	 */

	// TODO what if master changes during this?
}

/*
 * Received a controlsync.
 * If we are a slave upstreaming the same controlsync we can quit,
 * since either:
 * - master received it and is downstreaming to us
 * - or another slave member is also upstreaming.
 */
void cancelAnyUpstreamingInProgress() {
	// If not master and isActive, is upstreaming
	if (( ! SyncAgent::isSelfMaster()) and IntraCliqueManager::isActive())
		IntraCliqueManager::abort();
}


}



void NetworkTopology::subscribeToProvisionings() {
	ProvisioningPublisher::subscribe(2, NetworkTopology::handleNetGranularityProvisioning);
	ProvisioningPublisher::subscribe(3, NetworkTopology::handleScatterProvisioning);
}






/*
 * Slaves do not set control clique directly,
 * but relay to Master and wait till Master tells them.
 */


void NetworkTopology::handleNetGranularityProvisioning(uint32_t provisionedValue) {
	// cast raw OTA provisionedValue
	NetGranularity granularity = static_cast<NetGranularity>(provisionedValue);

	if (SyncAgent::isSelfMaster()) {
		masterTellCliqueMembers(granularity);
	}
	else {
		/*
		 * We are slave, relay upstream to master.
		 */
		IntraCliqueManager::doUpstreamCliqueSizeChange(granularity);
	}

}

void NetworkTopology::handleScatterProvisioning(uint32_t  provisionedValue) {
	/*
	 * provisionedValue is a signal: value not used.
	 */
	(void) provisionedValue;

	if (SyncAgent::isSelfMaster()) {
		// TODO same as above, don't scatter self until slaves told
		IntraCliqueManager::doDownstreamScatter();
	}
	else
		IntraCliqueManager::doUpstreamScatter();
}



void NetworkTopology::handleNetGranularityMessage(SyncMessage* msg) {
	if (clique.isMsgFromMyClique(msg->masterID)) {
		// assert msg from my clique (master or slave)

		// cast raw OTA provisionedValue
		NetGranularity granularity = static_cast<NetGranularity>(msg->work);

		if (SyncAgent::isSelfMaster()) {
			masterTellCliqueMembers(granularity);
		}
		else {
			cancelAnyUpstreamingInProgress();
			Granularity::setGranularity(granularity);
		}
	}
	// else ignore
}


void NetworkTopology::handleScatterMessage(SyncMessage* msg) {
	// TODO
}


