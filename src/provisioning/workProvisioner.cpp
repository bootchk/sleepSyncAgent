
#include "workProvisioner.h"


#include "../syncAgent/syncAgent.h"
#include "../network/intraCliqueManager.h"
#include "workControlProxy.h"


namespace {

ConverterFunc elapsedToAdvanceConverterFunc = nullptr;


void provisionWorkCycle(uint32_t workCycle) {
	if (SyncAgent::isSelfMaster()) {
		WorkControlProxy::getWorkCycleControlCallback()(workCycle);
	}
	else {
		// Upstream
		IntraCliqueManager::doUpstreamWorkCycle(workCycle);
	}
}

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
		WorkControlProxy::getWorkTimeControlCallback()(clockAdvance);
	}
	else {
		// Upstream
		IntraCliqueManager::doUpstreamWorkTime(clockAdvance);
	}
}



} // namespace


void WorkProvisioner::setConverterFunc(ConverterFunc aConverterFunc) {
	elapsedToAdvanceConverterFunc = aConverterFunc;
}


void WorkProvisioner::subscribeToProvisionings() {
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
