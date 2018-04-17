
#include "task.h"

#include "../schedule/radioPrelude.h"
#include "../schedule/syncSchedule.h"

#include "../syncAgentImp/state/syncMode.h"

#include "../slots/sync/syncSlot.h"
#include "../slots/syncing/syncSlotStep.h"

#include "../slots/fishing/fishingManager.h"

#include "../logging/logger.h"

#include <cassert>


namespace {

/*
 * Schedule task in the normally sleeping slots.
 *
 * Nonsync tasks are:
 * - fish (trolling or deep)
 * - merge
 * - provision
 * - none (maintain sync, but no power for any other task.)
 */
void scheduleNonSyncTask() {

	/*
	 * preFishing() decides next fish slot if any.
	 * Must precede tryUndoAfterSyncing() which uses the decision.
	 */
	FishingManager::preFishing();
	RadioPrelude::tryUndoAfterSyncing();


	// Prelude might be done, but radio not in use
	switch(SyncModeManager::mode()) {

	case SyncMode::SyncOnly:
		SyncSchedule::syncSlotAfterSyncSlot();
		break;
	case SyncMode::SyncAndFishMerge:
		SSTask::tryFishOrMerge();
		break;
	case SyncMode::SyncAndProvision:
		SyncSchedule::provisionStart();
		break;
	case SyncMode::Maintain:
		// should not get here
		// alternative to exception is: reset mode and schedule next sync point
		assert(false);
		break;
	}
}

}	// namespace


void SSTask::sendSync() {
	switch(SyncSlot::kind()) {
	case SyncSlotKind::sendWorkSync:
		SyncSlotStep::sendWorkSync();
		break;
	case SyncSlotKind::sendSync:
		SyncSlotStep::sendSync();
		break;
	case SyncSlotKind::sendControlSync:
		SyncSlotStep::sendControlSync();
		break;
	case SyncSlotKind::listen:
		// should not happen
		assert(false);
		break;
	}

	// Radio peripheral will send and power down
	// FUTURE we can do this now, earlier than what is coded now:  RadioPrelude::tryUndoAfterSyncing();

	// Next task is syncSlotEnd since it has a callback to app
	// Alternatively, we could schedule next fishing task.
	SyncSchedule::syncSlotEndFromSend();
}


/*
 * Listening sync slot is over, but radio might not be active.
 */
void SSTask::endSyncSlotListen() {
	// stop receiving and other bookkeeping
	SyncSlot::endListen();

	scheduleNonSyncTask();
}



void SSTask::endSyncSlotSend() {
	/*
	 * Radio not active, no events/tasks expected from radio.
	 * But RP is active.  (FUTURE, turn it off earlier.)
	 */
	assert(RadioPrelude::isDone());
	scheduleNonSyncTask();
}


/*
 * Always scheduled as a pair with the following task, so this is called directly, not on timer.
 */
void SSTask::radioPrelude() {
	// Start radio prelude (task on peripheral HFXO finishes later, but in finite time)
	RadioPrelude::doIt();
}


void SSTask::startSyncPeriodMaintain() {
	assert(not RadioPrelude::isDone());

	/*
	 * Call back app and try transition
	 */
	SyncSlot::bookkeepingAtPreludeToSyncSlot();

	SyncSlot::bookkeepingAtStartSyncSlot();

	/*
	 * We can't do this syncperiod, because RadioPrelude not done for SyncSlot.
	 * But if mode has changed, we can schedule RadioPrelude and usual, syncing syncPeriod.
	 */
	switch(SyncModeManager::mode()) {
	case SyncMode::Maintain:
		// sleep entire sync period
		SyncSchedule::maintainSyncPeriod();
		break;

	case SyncMode::SyncOnly:
	case SyncMode::SyncAndFishMerge:
	case SyncMode::SyncAndProvision:
		// Mode changed.  Next task is prelude
		SyncSchedule::radioPreludeTaskWSync();
		break;
	}
}


void SSTask::startSyncSlotWithoutScheduledPrelude() {
	// Same, the only difference is we are coming from an abutting slot with RadioPrelude done
	startSyncSlotAfterPrelude();
	// assert some task is scheduled
}


void SSTask::startSyncSlotAfterPrelude() {
	Logger::logSyncPoint();
	assert(RadioPrelude::isDone());


	SyncSlot::bookkeepingAtStartSyncSlot();

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
	// assert some task is scheduled
}





