
#include "sleepDuration.h"
#include "../slots/syncing/syncSlotSchedule.h"
#include "radioPrelude.h"

#include <cassert>
#include "syncSchedule.h"

#include "../task/task.h"
#include "taskTimer.h"

#include "../slots/fishing/fishingManager.h"

#include "../logging/logger.h"




namespace {

/*
 * Intermediates between RadioPrelude and continuation task
 */
void theRadioPreludeTaskWSync() {
	// Schedule next task from now
	SyncSchedule::startSyncSlotFromPreludeStart();

	SSTask::radioPrelude();
}

void theRadioPreludeTaskWFish() {
	// Schedule next task from now
	SyncSchedule::startFishSlotFromPreludeStart();

	SSTask::radioPrelude();
}

}	// namespace




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
	Logger::log(" SyncMaintain ");
	TaskTimer::schedule(SSTask::startSyncPeriodMaintain,
			SleepDuration::nowTilSyncPoint());
}




void SyncSchedule::syncSlotAfterSyncSlot() {
	assert(!RadioPrelude::isDone());
	SyncSchedule::radioPreludeTaskWSync();
}

void SyncSchedule::radioPreludeTaskWSync() {
	Logger::log(" PreludeWSync ");
	TaskTimer::schedule(theRadioPreludeTaskWSync,
			SleepDuration::nowTilPreludeWSync());
}

void SyncSchedule::startSyncSlotFromPreludeStart() {
	// Not assert RadioPrelude::isDone() because we schedule this just before RP::do()
	Logger::log(" StartFPrelude ");
	TaskTimer::schedule(SSTask::startSyncSlotAfterPrelude,
			SleepDuration::preludeTilNextTask());
}



// Prelude still done, don't need to schedule it
void SyncSchedule::startSyncSlotWithoutScheduledPrelude() {
	Logger::log(" StartWoPrelude ");
	TaskTimer::schedule(SSTask::startSyncSlotWithoutScheduledPrelude,
				SleepDuration::nowTilSyncPoint());
}

void SyncSchedule::syncSendTask() {
	Logger::log(" SendFStart ");
	TaskTimer::schedule(SSTask::sendSync,
			SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot());
}

void SyncSchedule::syncSlotEndFromListen() {
	Logger::log(" EndFListen ");
	TaskTimer::schedule(SSTask::endSyncSlotListen,
			SyncSlotSchedule::deltaToThisSyncSlotEnd());
}

void SyncSchedule::syncSlotEndFromSend() {
	Logger::log(" EndFSend ");
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
	Logger::log(" PreludeWFish ");
	TaskTimer::schedule(theRadioPreludeTaskWFish,
			SleepDuration::nowTilPreludeWFish());
}

void SyncSchedule::startFishSlotFromPreludeStart() {
	Logger::log(" FishFPrelude ");
	TaskTimer::schedule(SSTask::fishSlotStart,
			SleepDuration::preludeTilNextTask());
}

void SyncSchedule::fishSlotStart() {

	assert(RadioPrelude::isDone());
	// But we are not necessarily at end of RadioPrelude task
	Logger::log(" FishWPreludeDone ");
	TaskTimer::schedule(SSTask::fishSlotStart,
			SleepDuration::nowTilFishStart());
}

void SyncSchedule::fishSlotEnd() {

	// Duration varies
	Logger::log(" FishEndFStart ");
	TaskTimer::schedule(SSTask::fishSlotEnd,
			FishingManager::getFishSessionDuration());
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



void SyncSchedule::provisionStart() {
	Logger::log(" Provision ");
	TaskTimer::schedule(SSTask::provisionStart,
					SyncSlotSchedule::deltaToThisSyncSlotEnd());	// TODO proper time LOW
}

void SyncSchedule::merger() {
	Logger::log(" Merger ");
	TaskTimer::schedule(SSTask::mergerStart,
					SyncSlotSchedule::deltaToThisSyncSlotEnd());	// TODO proper time LOW
}
