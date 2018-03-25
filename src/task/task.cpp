
#include "task.h"

#include "../schedule/radioPrelude.h"
#include "../schedule/syncSchedule.h"

#include "../syncAgentImp/syncAgentImp.h"
#include "../syncAgentImp/state/syncMode.h"

// TODO one is old design
#include "../slots/sync/syncSlot.h"
#include "../slots/syncing/syncWorkSlot.h"

//#include "../modules/syncSender.h"

#include <cassert>




void SSTask::sendSync() {
	switch(SyncSlot::kind()) {
	case SyncSlotKind::sendWorkSync:
		SyncWorkSlot::sendWorkSync();
		break;
	case SyncSlotKind::sendSync:
		SyncWorkSlot::sendSync();
		break;
	case SyncSlotKind::sendControlSync:
		SyncWorkSlot::sendControlSync();
		break;
	case SyncSlotKind::listen:
		// should not happen
		break;
	}

	// Radio peripheral will send and power down

	// TODO schedule next task
	// TODO if don't need radio for next task, RadioPrelude::undo()
	// switch on prelude shut down
	// SyncSchedule::syncSlotEnd();
}


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
