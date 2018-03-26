
#include "timer.h"
#include "sleepDuration.h"
#include "../slots/syncing/syncSlotSchedule.h"
#include "radioPrelude.h"

#include <cassert>
#include "syncSchedule.h"

#include "../task/task.h"


namespace {

/*
 * Intermediates between RadioPrelude and continuation task
 */
void theRadioPreludeTaskWSync() {
	// Schedule next task from now
	SyncSchedule::syncTaskFromPreludeStart();

	SSTask::radioPrelude();
}

void theRadioPreludeTaskWFish() {
	// Schedule next task from now
	SyncSchedule::fishTaskFromPreludeStart();

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



void SyncSchedule::syncSlotAfterSyncSlot() {
	assert(!RadioPrelude::isDone());
	SyncSchedule::radioPreludeTaskWSync();
}

void SyncSchedule::radioPreludeTaskWSync() {
	Timer::schedule(theRadioPreludeTaskWSync,
			SleepDuration::nowTilPreludeWSync());
}

void SyncSchedule::syncTaskFromPreludeStart() {
	Timer::schedule(SSTask::startSyncSlotAfterPrelude,
			SleepDuration::preludeTilNextTask());
}

void SyncSchedule::maintainSyncPeriodFromMaintainSyncPeriod() {
	// Sleeping almost entire period without sync slot
	Timer::schedule(SSTask::startSyncPeriodMaintain,
			SleepDuration::nowTilSyncPoint());
}

// Prelude still done, don't need to schedule it
void SyncSchedule::startSyncSlotWithoutScheduledPrelude() {
	Timer::schedule(SSTask::startSyncSlotWithoutScheduledPrelude,
				SleepDuration::nowTilSyncPoint());
}

void SyncSchedule::syncSendTask() {
	Timer::schedule(SSTask::sendSync,
			SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot());
}

void SyncSchedule::syncSlotEndListen() {
	Timer::schedule(SSTask::endSyncSlotListen,
			SyncSlotSchedule::deltaToThisSyncSlotEnd());
}

void SyncSchedule::syncSlotEndSend() {
	Timer::schedule(SSTask::endSyncSlotSend,
			SyncSlotSchedule::deltaToThisSyncSlotEnd());
}



/*
 * Nonsync slots
 */

void SyncSchedule::omitNonsyncSlot() {
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
	Timer::schedule(theRadioPreludeTaskWFish,
			SleepDuration::nowTilPreludeWFish());
}

void SyncSchedule::fishTaskFromPreludeStart() {
	Timer::schedule(SSTask::fishSlotStart,
			SleepDuration::preludeTilNextTask());
}

void SyncSchedule::fishSlotStart() {
	assert(RadioPrelude::isDone());
	// But we are not necessarily at end of RadioPrelude task
	Timer::schedule(SSTask::fishSlotStart,
				SyncSlotSchedule::deltaToThisSyncSlotEnd());	// TODO proper time
}

void SyncSchedule::fishSlotEnd() {
	Timer::schedule(SSTask::fishSlotEnd,
				SyncSlotSchedule::deltaToThisSyncSlotEnd());	// TODO proper time
}





void SyncSchedule::provisionStart() {
	Timer::schedule(SSTask::provisionStart,
					SyncSlotSchedule::deltaToThisSyncSlotEnd());	// TODO proper time
}

void SyncSchedule::merger() {
	Timer::schedule(SSTask::mergerStart,
					SyncSlotSchedule::deltaToThisSyncSlotEnd());	// TODO proper time
}
