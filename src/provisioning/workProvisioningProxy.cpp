
#include "workProvisioningProxy.h"

#include "../network/intraCliqueManager.h"
#include "../syncAgent/syncAgent.h"

#include "../globals.h"
#include "../clique/clique.h"

namespace {

ProvisionCallback finalWorkTimeProvisioningCallback = nullptr;
ProvisionCallback finalWorkCycleProvisioningCallback = nullptr;

ConverterFunc elapsedToAdvanceConverterFunc = nullptr;




/*
 * Called at initial provisioning.
 * If Master, provision directly, else upstream to Master.
 */
void provisionWorkTime(uint32_t  periodsElapsed) {
	/*
	 * Fix wall time periodsElapsed to clock advance on distributed work clock.
	 * We must do this now, while periodsElapsed is not stale.
	 * This is a conversion between clocks.
	 */
	uint32_t clockAdvance = elapsedToAdvanceConverterFunc(periodsElapsed);

	if (SyncAgent::isSelfMaster()) {
		finalWorkTimeProvisioningCallback(clockAdvance);
	}
	else {
		// Upstream
		IntraCliqueManager::doUpstreamWorkTime(clockAdvance);
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


bool isMsgForSelfAsMaster(SyncMessage* msg) {
	return ( clique.isMsgFromMyClique(msg->masterID)
			and SyncAgent::isSelfMaster()
);
}


}	// namespace




void WorkProvisioningProxy::setWorkTimeFinalProvisioningCallback(ProvisionCallback aCallback) {
	finalWorkTimeProvisioningCallback = aCallback;
}

void WorkProvisioningProxy::setWorkCycleFinalProvisioningCallback(ProvisionCallback aCallback) {
	finalWorkCycleProvisioningCallback = aCallback;
}

void WorkProvisioningProxy::setConverterFunc(ConverterFunc aConverterFunc) {
	elapsedToAdvanceConverterFunc = aConverterFunc;
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



void WorkProvisioningProxy::handleWorkTimeMessage(SyncMessage* msg) {
	// TODO is network provisioning checking for masterID?
	if ( isMsgForSelfAsMaster(msg) ) {
		/*
		 * Self owns master DistributedWorkClock.
		 * Sync it and soon will start distributing it via WorkSync.
		 */
		// unsigned char silently coerced to unsigned int
		finalWorkTimeProvisioningCallback(msg->work);
	}
}
void WorkProvisioningProxy::handleWorkCycleMessage(SyncMessage* msg) {
	if ( isMsgForSelfAsMaster(msg) ) {
		finalWorkCycleProvisioningCallback(msg->work);
	}
}
