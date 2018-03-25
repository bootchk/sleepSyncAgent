
#include "task.h"

#include "../schedule/radioPrelude.h"
#include "../schedule/syncSchedule.h"

#include "../syncAgentImp/syncAgentImp.h"
#include "../syncAgentImp/state/syncMode.h"

#include "../slots/syncing/syncSlot.h"

#include <cassert>






/*
 * Always scheduled as a pair with the following task, so this is called directly, not on timer.
 */
void SSTask::radioPrelude() {
	// Start radio prelude (task on peripheral HFXO finishes later, but in finite time)
	RadioPrelude::doIt();
}

// TODO if maintain, don't do radioprelude

void SSTask::startSyncSlotAfterPrelude() {
	assert(RadioPrelude::isDone());
	// bookkeeping
	SyncAgentImp::preludeToSyncPeriod();

	SyncModeManager::checkPowerAndTryModeTransitions();

	switch(SyncModeManager::mode()) {
	case SyncMode::Maintain:
		// Even though we already did RadioPrelude, can't do SyncSlot
		SyncSchedule::maintainSyncPeriod();
		RadioPrelude::undo();
		break;

	case SyncMode::SyncOnly:
	case SyncMode::SyncAndFishMerge:
	case SyncMode::SyncAndProvision:
		// Next task is sync slot of some kind, listen or send
		SyncSlot::dispatchSyncSlotKind();
		break;
	}
}
