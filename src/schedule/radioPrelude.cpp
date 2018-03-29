
#include "radioPrelude.h"

#include "../slots/fishing/fishingManager.h"
#include "../slots/merging/mergeSchedule.h"

// Nested state
#include "../syncAgentImp/state/syncMode.h"
#include "../syncAgentImp/state/role.h"

// Not using radioSoC Ensemble, because it sleeps during prelude
#include <clock/clockFacilitator.h>

#include "../logging/logger.h"
#include <cassert>



/*
 * State kept, since Ensemble doesn't keep appropriate state.
 * Ensemble might tell us prelude is over, but not whether started.
 */
namespace {
	bool isStarted = false;
}

void  RadioPrelude::doIt() {
	Logger::log(" RP^ ");
	ClockFacilitator::startHFXONoWait();
	isStarted = true;
}


void  RadioPrelude::undo() {
	Logger::log(" RPv ");
	ClockFacilitator::stopHFXO();
	isStarted = false;
}


bool RadioPrelude::isDone() { return isStarted; }


// TODO condense these to one
bool RadioPrelude::tryUndoAfterSyncing() {
	bool result;

	result = shouldUndoAfterSyncing();
	if (result) undo();
	return result;
}

bool RadioPrelude::tryUndoAfterFishing() {
	bool result;

	result = shouldUndoAfterFishing();
	if (result) undo();
	return result;
}

bool RadioPrelude::tryUndoAfterMerging() {
	bool result;

	result = shouldUndoAfterMerging();
	if (result) undo();
	return result;
}



/*
 * These assume sync slot is always first in syncPeriod.
 * So a just completed fishing/merging at end of syncPeriod is near next sync slot.
 * And about to be done fishing near beginning of syncPeriod is just after sync slot.
 */


bool RadioPrelude::shouldUndoAfterSyncing() {
	// Call here is not needed if RP is not done
	assert(isDone());

	bool result;
	switch(SyncModeManager::mode()) {
	case SyncMode::Maintain:
		assert(false);
		break;

	case SyncMode::SyncAndFishMerge:
		switch(MergerFisherRole::role()) {
		case Role::Fisher:
			// One more level of state: Trolling/Deep handled polymorphically
			result = not FishingManager::isFishSlotStartSyncPeriod();
			break;
		case Role::Merger:
			result = not MergeSchedule::isMergerStartSyncPeriod();
			break;
		case Role::NoFishing:
			result = true;
		}
		break;

	case SyncMode::SyncOnly:	// no non-sync slot follows
	case SyncMode::SyncAndProvision:	// provision not use RP
		result = true;
	}
	return result;
}

bool RadioPrelude::shouldUndoAfterFishing() { return not FishingManager::isFishSlotEndSyncPeriod(); }
bool RadioPrelude::shouldUndoAfterMerging() { return not MergeSchedule::isMergerEndSyncPeriod(); }
