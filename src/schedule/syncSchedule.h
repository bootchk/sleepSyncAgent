
#pragma once

/*
 * Knows what to schedule next.
 * Understands slot sequence and radio prelude.
 */
class SyncSchedule {
public:
	static void initialSyncPeriod();

	static void syncSlotAfterNullSyncSlot();
	static void syncSlotAfterSyncSlot();
	static void radioPreludeTaskWSync();
	static void syncTaskFromPreludeStart();
};
