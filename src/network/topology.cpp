
#include "topology.h"

#include "granularity.h"
#include "scatter.h"

#include "../globals.h"  // clique
#include "intraCliqueManager.h"

#include "../provisioning/provisioningPublisher.h"

#include "../clique/clique.h"
#include "../syncAgentImp/syncAgentImp.h"

#include "../logging/logger.h"


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

	NetGranularity granularity = Granularity::getFromRaw(static_cast<unsigned char> (provisionedValue));
	if (granularity == NetGranularity::Invalid) {
		Logger::log("Provisioned granularity invalid\n");
		return;
	}

	if (SyncAgentImp::isSelfMaster()) {
		masterTellCliqueGranularityChange(granularity);
	}
	else {
		// We are slave, relay upstream to master.
		// Will go OTA and could get garbled again.
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
	NetGranularity granularity = Granularity::getFromRaw(msg->work);
	if (granularity == NetGranularity::Invalid) {
		// already logged during conversion
		return;
	}

	if (clique.isMsgFromMyClique(msg->masterID)) {
		// in most designs, is upstream from slave
		if (SyncAgentImp::isSelfMaster()) {
			masterTellCliqueGranularityChange(granularity);
		}
		else {
			/*
			 * Self is slave.
			 * If upstream and downstream are both ControlSyncGranularity,
			 * this could be another slave.
			 * If only upstream is ControlSyncGranularity, this must be another slave upstreaming.
			 *
			 * If we hear each other in the same syncSlot, small possibility we both cancel
			 * and we fail to upstream to master.
			 * Since provisioning is probabilistic anyway, disregard that possibility.
			 *
			 * As slave, we wait to hear from master:
			 * NOT Granularity::setGranularity(granularity);
			 */
			cancelAnyUpstreamingInProgress();
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


