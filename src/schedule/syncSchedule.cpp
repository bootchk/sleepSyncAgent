
#include "timer.h"
#include "sleepDuration.h"
#include "radioPrelude.h"

#include <cassert>
#include "syncSchedule.h"

#include "../task/task.h"


namespace {

void theRadioPreludeTaskWSync() {
	// Schedule next task from now
	SyncSchedule::syncTaskFromPreludeStart();

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
			SleepDuration::preludeTilSync());
}

void SyncSchedule::maintainSyncPeriodFromMaintainSyncPeriod() {
	Timer::schedule(SSTask::startSyncSlotWithoutPrelude,
			SleepDuration::nowTilSyncPoint());
}



void SyncSchedule::syncSendTask() {
	// TODO
	Timer::schedule(SSTask::startSyncSlotWithoutPrelude,
			SleepDuration::nowTilSyncPoint());
}

void SyncSchedule::syncSlotEndListen() {
	// TODO
	Timer::schedule(SSTask::startSyncSlotWithoutPrelude,
			SleepDuration::nowTilSyncPoint());
}


