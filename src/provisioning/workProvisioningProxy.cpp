
#include "workProvisioningProxy.h"

#include "../network/intraCliqueManager.h"
#include "../syncAgent/syncAgent.h"


namespace {

ProvisionCallback finalWorkTimeProvisioningCallback = nullptr;
ProvisionCallback finalWorkCycleProvisioningCallback = nullptr;

/*
 * Called at initial provisioning.
 * If Master, provision directly, else upstream to Master.
 */
void provisionWorkTime(uint32_t  periodsElapsed) {
	// TODO conversion
	if (SyncAgent::isSelfMaster()) {

		finalWorkTimeProvisioningCallback(periodsElapsed);
	}
	else {
		// Upstream
		IntraCliqueManager::doUpstreamWorkTime(periodsElapsed);
		// TODO arrange final provisioning callback is null
	}
}


void provisionWorkCycle(uint32_t workCycle) {
	if (SyncAgent::isSelfMaster()) {

		finalWorkCycleProvisioningCallback(workCycle);
	}
	else {
		// Upstream
		IntraCliqueManager::doUpstreamWorkCycle(workCycle);
	}
}


}

void WorkProvisioningProxy::setWorkTimeFinalProvisioningCallback(ProvisionCallback aCallback) {
	finalWorkTimeProvisioningCallback = aCallback;
}

void WorkProvisioningProxy::setWorkCycleFinalProvisioningCallback(ProvisionCallback) {
	finalWorkCycleProvisioningCallback = aCallback;
}



void WorkProvisioningProxy::subscribeToProvisionings() {
	/*
	 * Subscribe with callbacks to self.
	 */
	ProvisioningPublisher::subscribe(
			ProvisionablePropertyIndex::WorkTime,
			provisionWorkTime);

	ProvisioningPublisher::subscribe(
			ProvisionablePropertyIndex::WorkCycle,
			provisionWorkCycle);
}
