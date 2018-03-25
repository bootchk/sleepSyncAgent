
#include "timer.h"
#include "sleepDuration.h"
#include "radioPrelude.h"

#include <cassert>
#include "syncSchedule.h"



namespace {

void radioPreludeTask() {
	// Start radio prelude (task on peripheral HFXO finishes later, but in finite time)
	RadioPrelude::doIt();
}

void syncTask() {
	// rollover
	// callbacks
	// radio on
}

void theRadioPreludeTaskWSync() {
	// Schedule next task from now
	SyncSchedule::syncTaskFromPreludeStart();

	radioPreludeTask();
}

}	// namespace


void SyncSchedule::initialSyncPeriod() {
	// now is before any real slot have been done, but clock and schedule are running.
	// Just schedule a syncslot as if we just did one.
	SyncSchedule::syncSlotAfterSyncSlot();
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
	Timer::schedule(syncTask,
			SleepDuration::preludeTilSync());
}


