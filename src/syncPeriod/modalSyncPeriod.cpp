
#include "modalSyncPeriod.h"

#include <cassert>

#include "../syncAgentImp/state/syncMode.h"
#include "../sleepers/scheduleSleeper.h"
#include "syncPeriod.h"



void ModalSyncPeriod::perform() {

	SyncModeManager::checkPowerAndTryModeTransitions();

	switch(SyncModeManager::mode()) {
	case SyncMode::Maintain:
		ScheduleSleeper::sleepEntireSyncPeriod();
		break;

	/*
	 * CombinedSyncPeriod also checks FisherMergerRole (sub mode.)
	 */
	case SyncMode::SyncOnly:
		CombinedSyncPeriod::doSlotSequence();
		break;
	case SyncMode::SyncAndFishMerge:
		CombinedSyncPeriod::doSlotSequence();
		break;

	case SyncMode::SyncAndProvision:
#ifdef BLE_PROVISIONED
		ProvisioningSyncPeriod::doSlotSequence();
#else
		assert(false);	// illegal to be in this state when not built for provisioning
#endif
			break;
	}
}
