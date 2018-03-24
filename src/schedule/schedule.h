
#pragma once

/*
 * Knows what to schedule next.
 * Understands slot sequence and radio prelude.
 */
class Schedule {
public:
	static void syncSlotAfterNullSyncSlot();
	static void syncSlotAfterSyncSlot();
	static void radioPreludeTaskWSync();
	static void syncTaskFromPreludeStart();
};
