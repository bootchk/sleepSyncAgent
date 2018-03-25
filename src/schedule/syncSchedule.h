
#pragma once

/*
 * Knows what to schedule next.
 * Understands slot sequence and radio prelude.
 */
class SyncSchedule {
public:
	/*
	 * now is before any real slot have been done, but clock and schedule are running.
	 * Schedule a null syncPeriod as if we just did one.
	 */
	static void initialSyncPeriod();

	/*
	 * Mode is Maintain.
	 * Schedule an empty syncPeriod, without RadioPrelude, then check mode again.
	 */
	static void maintainSyncPeriod();


	static void maintainSyncPeriodFromMaintainSyncPeriod();
	static void syncSlotAfterNullSyncSlot();
	static void syncSlotAfterSyncSlot();
	static void radioPreludeTaskWSync();
	static void syncTaskFromPreludeStart();

	// Waypoints of SyncSlot
	static void syncSendTask();
	static void syncSlotEndListen();
};
