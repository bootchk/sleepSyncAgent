
#pragma once

// OS
// static void scheduleTask(void callback() ) {}


#include "../message.h"

/*
 * Schedule is infinite sequence of periods, aligned with global clock of clique.
 * Period is sequence of slots: sync, work, sleeping, ...., sleeping
 *
 * Sleeping slots may be used for fishing or merging
 *
 * Implementation here is mainly about aligning with global clock.
 * Implementation of slotting is done by SyncAgent scheduling tasks.
 * The mcu and radio may be sleeping(low-power idle) during any slot, not just sleeping slots.
 * The OS schedules tasks using a low-power timer peripheral that never is off.
 *
 * Responsibilities:
 * - schedule tasks to run at slot start/end (interface to OS)
 */
class Schedule {

public:
	void adjustBySyncMsg(Message msg) {/*TODO*/ };

	// Scheduling slots tasks
	void scheduleEndSyncSlotTask(void callback()) {};
	void scheduleEndWorkSlotTask(void callback()) {};
	void scheduleEndFishSlotTask(void callback()) {};
	void scheduleEndMergeSlotTask(void callback()) {};

	void scheduleStartSyncSlotTask(void callback()) {};
	// Work slot follows sync without start callback
	void scheduleStartFishSlotTask(void callback()) {};
	void scheduleStartMergeSlotTask(void callback()) {};
private:

};
