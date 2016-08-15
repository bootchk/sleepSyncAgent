
#pragma once



#include "../message.h"
#include "longClock.h"

/*
 * Schedule is infinite sequence of periods, aligned with global clock of clique.
 * Period is sequence of slots: sync, work, sleeping, ...., sleeping
 *
 * Sleeping slots may be repurposed for fishing or merging.
 * At most one sleeping slot is repurposed per period
 * (so that use of electrical power is not bursty.)
 *
 * Implementation here is mainly about aligning with global clock.
 *
 * Knowledge (implementation) of slot sequence is spread through SyncAgent.
 * (callbacks know which slot should follow.)
 *
 * The mcu and radio may be sleeping(low-power idle) during any slot, not just sleeping slots.
 * The OS schedules tasks using a low-power timer peripheral that never is off.
 *
 * Because a message may be received after the intended end of a slot
 * (if a msg start is received during the slot and the radio is not turned while a msg is in progress.)
 * TODO ???
 *
 * Responsibilities:
 * - maintain period start time (in sync with members of clique)
 * - schedule tasks to run at slot start/end (interface to OS)
 */
class Schedule {

public:
	void adjustBySyncMsg(Message msg);

	// Scheduling slots tasks
	void scheduleEndSyncSlotTask(void callback());
	void scheduleEndWorkSlotTask(void callback());
	void scheduleEndFishSlotTask(void callback());
	void scheduleEndMergeSlotTask(void callback());

	void scheduleStartSyncSlotTask(void callback());
	// Work slot follows sync without start callback
	void scheduleStartFishSlotTask(void callback());
	void scheduleStartMergeSlotTask(void callback());
private:
	static LongClock longClock;

};
