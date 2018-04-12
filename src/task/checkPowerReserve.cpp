
#include "task.h"

#include "../modules/syncPowerManager.h"
#include "../syncAgentImp/syncAgentImp.h"
#include "../schedule/syncSchedule.h"


void SSTask::checkPowerReserve() {
	if (SyncPowerManager::isPowerForStartLoop()) {

		// Callback app so it can connect signals
		SyncAgentImp::onPowerReserveCallback();

		SyncAgentImp::init();

		// Get ready for loop on syncPeriods
		SyncAgentImp::preludeToLoop();

		SyncSchedule::initialSyncPeriod();
	}
	else {
		// Recurse, schedule self
		SyncSchedule::checkPowerReserve();
	}
}


