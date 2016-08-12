
#pragma once

#include "message.h"

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
 */
class Schedule {

public:
	void adjustBySyncMsg(Message msg) {/*TODO*/ };

private:

};
