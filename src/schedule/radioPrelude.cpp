
#include "radioPrelude.h"

#include "../slots/fishing/fishingManager.h"

// Not using radioSoC Ensemble, because it sleeps during prelude
#include <clock/clockFacilitator.h>

#include "../logging/logger.h"


/*
 * State kept, since Ensemble doesn't keep appropriate state.
 * Ensemble might tell us prelude is over, but not whether started.
 */
namespace {
	bool isStarted = false;
}

void  RadioPrelude::doIt() {
	Logger::log(" RP::do ");
	ClockFacilitator::startHFXONoWait();
	isStarted = true;
}


void  RadioPrelude::undo() {
	Logger::log(" RP::undo ");
	ClockFacilitator::stopHFXO();
	isStarted = false;
}


bool RadioPrelude::isDone() { return isStarted; }


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




/*
 * These assume sync slot is always first in syncPeriod.
 * So at just completed fishing at end of syncPeriod is near next sync slot.
 * And about to be done fishing near beginning of syncPeriod is just after sync slot.
 */
bool RadioPrelude::shouldUndoAfterFishing() { return not FishingManager::isFishSlotEndSyncPeriod(); }
bool RadioPrelude::shouldUndoAfterSyncing() { return not FishingManager::isFishSlotStartSyncPeriod(); }
