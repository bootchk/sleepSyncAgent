
#include "topology.h"

#include "granularity.h"
#include "scatter.h"

#include "../globals.h"  // clique
#include "intraCliqueManager.h"

#include "../provisioning/provisioningPublisher.h"

#include "../clique/clique.h"
#include "../syncAgentImp/syncAgentImp.h"




namespace {

void masterTellCliqueGranularityChange(NetGranularity granularity) {

	IntraCliqueManager::doDownstreamCliqueSizeChange(granularity);

	/*
	 * Master set its own granularity later
	 * !!! We keep our current xmit power while we tell slaves,
	 * and then change (dec or inc) our own xmit power.
	 * Spoofing?
	 */

	// TODO what if master changes during this?
}


void masterTellCliqueToScatter() {
	IntraCliqueManager::doDownstreamScatter();
	/*
	 * We stay in sync and master until later
	 * OW we would not communicate with slaves.
	 */
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
	if (( ! SyncAgentImp::isSelfMaster()) and IntraCliqueManager::isActive())
		IntraCliqueManager::abort();
}


}



void NetworkTopology::subscribeToProvisionings() {
	ProvisioningPublisher::subscribe(
			ProvisionablePropertyIndex::NetGranularity,
			NetworkTopology::handleNetGranularityProvisioning);
	ProvisioningPublisher::subscribe(
			ProvisionablePropertyIndex::Scatter,
			NetworkTopology::handleScatterProvisioning);
}






/*
 * Slaves do not set control clique directly,
 * but relay to Master and wait till Master tells them.
 */


void NetworkTopology::handleNetGranularityProvisioning(uint32_t provisionedValue) {
	// cast raw OTA provisionedValue
	NetGranularity granularity = static_cast<NetGranularity>(provisionedValue);

	if (SyncAgentImp::isSelfMaster()) {
		masterTellCliqueGranularityChange(granularity);
	}
	else {
		// We are slave, relay upstream to master.
		IntraCliqueManager::doUpstreamCliqueSizeChange(granularity);
	}

}

void NetworkTopology::handleScatterProvisioning(uint32_t  provisionedValue) {
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



void NetworkTopology::handleNetGranularityMessage(SyncMessage* msg) {
	if (clique.isMsgFromMyClique(msg->masterID)) {
		// assert msg from my clique (master or slave)

		// cast raw OTA provisionedValue
		NetGranularity granularity = static_cast<NetGranularity>(msg->work);

		if (SyncAgentImp::isSelfMaster()) {
			masterTellCliqueGranularityChange(granularity);
		}
		else {
			// TODO if this is also an upstreaming message and we both cancel before master hears?
			// Need a separate downstream msg type?
			cancelAnyUpstreamingInProgress();
			Granularity::setGranularity(granularity);
		}
	}
	// else ignore
}



void NetworkTopology::handleScatterMessage(SyncMessage* msg) {
	/*
	 * Assert message is in range
	 */

	// msg content not used, is a signal
	(void) msg;

	/*
	 * Same logic as for NetGranularity
	 */
	if (clique.isMsgFromMyClique(msg->masterID)) {
			// assert msg from my clique (master or slave)

			if (SyncAgentImp::isSelfMaster()) {
				masterTellCliqueToScatter();
			}
			else {
				// Assume upstreaming the same provisioned message: sactter
				cancelAnyUpstreamingInProgress();

				Scatter::scatter();
			}
		}
		// else ignore

}


