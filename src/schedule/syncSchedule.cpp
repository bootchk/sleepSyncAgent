
#include "sleepDuration.h"
#include "../slots/syncing/syncSlotSchedule.h"
#include "../slots/sync/syncSlot.h"
#include "radioPrelude.h"

#include <cassert>
#include "syncSchedule.h"

#include "../task/task.h"


#include "../slots/fishing/fishingManager.h"
#include "../logging/logger.h"
#include "../slots/merge/mergeSchedule.h"
#include "../provisioning/provisionSchedule.h"

// radioSoC
#include <clock/taskTimer.h>





namespace {

/*
 * Intermediates between RadioPrelude and continuation task (Fish or Sync)
 */

void theRadioPreludeTaskWSync() {
	// Schedule next task from now, before doing this task
	SyncSchedule::startSyncSlotFromPreludeStart();


	SSTask::radioPrelude();

	/*
	 * We have one slot (say 50 ticks) that is essentially idle,
	 * waiting for HFXO to start.
	 * Do tasks that must immediately precede the syncSlot.
	 */
	SyncSlot::bookkeepingAtPreludeToSyncSlot();
}

void theRadioPreludeTaskWFish() {
	// Schedule next task from now
	SyncSchedule::startFishSlotFromPreludeStart();

	SSTask::radioPrelude();
}

}	// namespace








void SyncSchedule::checkPowerReserve() {
	TaskTimer::schedule(SSTask::checkPowerReserve,
				SleepDuration::powerCheckInterval());
}


/*
 * SyncPeriod without prior RadioPrelude
 */
void SyncSchedule::initialSyncPeriod() {
	SyncSchedule::maintainSyncPeriodFromMaintainSyncPeriod();
}

void SyncSchedule::maintainSyncPeriod() {
	SyncSchedule::maintainSyncPeriodFromMaintainSyncPeriod();
}

void SyncSchedule::maintainSyncPeriodFromMaintainSyncPeriod() {

	// Sleeping almost entire period without sync slot
	Logger::log(" SyncMaintain");
	TaskTimer::schedule(SSTask::startSyncPeriodMaintain,
			SleepDuration::nowTilSyncPoint());
}




void SyncSchedule::syncSlotAfterSyncSlot() {
	assert(!RadioPrelude::isDone());
	SyncSchedule::radioPreludeTaskWSync();
}

void SyncSchedule::radioPreludeTaskWSync() {
	Logger::log(" PreWSync");
	TaskTimer::schedule(theRadioPreludeTaskWSync,
			SleepDuration::nowTilPreludeWSync());
}

void SyncSchedule::startSyncSlotFromPreludeStart() {
	// Not assert RadioPrelude::isDone() because we schedule this just before RP::do()
	Logger::log(" StrtFPre");
	TaskTimer::schedule(SSTask::startSyncSlotAfterPrelude,
			SleepDuration::preludeTilNextTask());
}



// Prelude still done, don't need to schedule it
void SyncSchedule::startSyncSlotWithoutScheduledPrelude() {
	Logger::log(" StrtWoPre");
	TaskTimer::schedule(SSTask::startSyncSlotWithoutScheduledPrelude,
				SleepDuration::nowTilSyncPoint());
}

void SyncSchedule::syncSendTask() {
	Logger::log(" SndFStrt");
	TaskTimer::schedule(SSTask::sendSync,
			SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot());
}

void SyncSchedule::syncSlotEndFromListen() {
	Logger::log(" EndFLstn");
	TaskTimer::schedule(SSTask::endSyncSlotListen,
			SyncSlotSchedule::deltaToThisSyncSlotEnd());
}

void SyncSchedule::syncSlotEndFromSend() {
	Logger::log(" EndFSend");
	TaskTimer::schedule(SSTask::endSyncSlotSend,
			SyncSlotSchedule::deltaToThisSyncSlotEnd());
}



/*
 * Nonsync slots
 */

void SyncSchedule::omitNonsyncSlot() {
	/*
	 * Skip entire nonsyncPeriod.
	 */
	// RadioPrelude might be done.  Ensure it is not done.
	RadioPrelude::undo();
	SyncSchedule::syncSlotAfterSyncSlot();
}



/*
 * Fishing nonsync slot
 */
void SyncSchedule::fishing() {
	if (RadioPrelude::isDone()) {
		// Fishing is near in time
		SyncSchedule::fishSlotStart();
	}
	else {
		SyncSchedule::radioPreludeTaskWFish();
	}
}

void SyncSchedule::radioPreludeTaskWFish() {
	Logger::log(" PreWFish");
	TaskTimer::schedule(theRadioPreludeTaskWFish,
			SleepDuration::nowTilPreludeWFish());
}

void SyncSchedule::startFishSlotFromPreludeStart() {
	Logger::log(" FishFPre");
	TaskTimer::schedule(SSTask::fishSlotStart,
			SleepDuration::preludeTilNextTask());
}

void SyncSchedule::fishSlotStart() {

	assert(RadioPrelude::isDone());
	// But we are not necessarily at end of RadioPrelude task
	Logger::log(" FishWPreDone");
	TaskTimer::schedule(SSTask::fishSlotStart,
			SleepDuration::nowTilFishStart());
}

void SyncSchedule::fishSlotEnd() {

	// Duration varies
	Logger::log(" FishEndFStrt");
	TaskTimer::schedule(SSTask::fishSlotEnd,
			FishingManager::getFishSessionDurationTicks());
}



void SyncSchedule::syncSlotAfterFishSlot() {
	if (RadioPrelude::tryUndoAfterFishing()) {
		assert(not RadioPrelude::isDone());
		SyncSchedule::radioPreludeTaskWSync();
	}
	else {
		SyncSchedule::startSyncSlotWithoutScheduledPrelude();
	}
}



/*
 * Merging
 */

void SyncSchedule::merger() {
	Logger::log(" Merger");
	TaskTimer::schedule(SSTask::mergerStartAndEnd,
					MergeSchedule::deltaToThisMergeStart());
}

void SyncSchedule::syncSlotAfterMerging() {
	if (RadioPrelude::tryUndoAfterMerging()) {
		assert(not RadioPrelude::isDone());
		SyncSchedule::radioPreludeTaskWSync();
	}
	else {
		SyncSchedule::startSyncSlotWithoutScheduledPrelude();
	}
}


#ifdef SOFTDEVICE_PRESENT
/*
 * Provisioning.
 */
void SyncSchedule::provisionStart() {
	Logger::log(" ProvStrt");
	TaskTimer::schedule(SSTask::provisionStart,
					ProvisionSchedule::deltaToProvisionStart());
}


void SyncSchedule::provisionEnd() {
	Logger::log(" ProvEnd");
	TaskTimer::schedule(SSTask::provisionEnd,
					ProvisionSchedule::deltaToProvisionEnd());
}


void SyncSchedule::syncSlotAfterProvisioning() {
	Logger::log(" SyncFProv");
	if (RadioPrelude::tryUndoAfterMerging()) {
		assert(not RadioPrelude::isDone());
		SyncSchedule::radioPreludeTaskWSync();
	}
	else {
		SyncSchedule::startSyncSlotWithoutScheduledPrelude();
	}
}

#endif


