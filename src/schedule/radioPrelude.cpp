
#include "radioPrelude.h"

#include "../slots/fishing/fishingManager.h"
#include "../syncAgentImp/state/syncMode.h"
#include "../syncAgentImp/state/role.h"

// Not using radioSoC Ensemble, because it sleeps during prelude
#include <clock/clockFacilitator.h>

#include "../logging/logger.h"
#include <cassert>
#include "../slots/merge/mergeSchedule.h"



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

bool RadioPrelude::tryUndoAfterProvisioning() {
	bool result;

	result = shouldUndoAfterProvisioning();
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
		assert(false);	// Maintain mode should not call here.
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

	case SyncMode::SyncOnly:	// no non-sync slot abuts
		result = true;
		break;
	case SyncMode::SyncAndProvision:
		result = false;	// Provisioning abuts and uses RadioPrelude (HFXO)
		break;

	}
	return result;
}

bool RadioPrelude::shouldUndoAfterFishing() { return not FishingManager::isFishSlotEndSyncPeriod(); }
bool RadioPrelude::shouldUndoAfterMerging() { return not MergeSchedule::isMergerEndSyncPeriod(); }

/*
 * Provisioning start after SyncSlot and last one second.
 * Therefore, another second before next SyncSlot.
 * TODO should be in terms of other const parameters, assumptions made elsewhere.
 */
bool RadioPrelude::shouldUndoAfterProvisioning() { return true; }



