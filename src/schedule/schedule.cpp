
#include "schedule.h"

#include "timer.h"
#include "sleepDuration.h"
#include "radioPrelude.h"

#include <cassert>



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
	Schedule::syncTaskFromPreludeStart();

	radioPreludeTask();
}

}	// namespace


void Schedule::syncSlotAfterSyncSlot() {
	assert(!RadioPrelude::isDone());
	radioPreludeTaskWSync();
}

void Schedule::radioPreludeTaskWSync() {
	Timer::schedule(theRadioPreludeTaskWSync,
			SleepDuration::nowTilPreludeWSync());
}

void Schedule::syncTaskFromPreludeStart() {
	Timer::schedule(syncTask,
			SleepDuration::preludeTilSync());
}


