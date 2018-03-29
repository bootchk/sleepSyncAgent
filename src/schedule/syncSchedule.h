
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

	// to SyncSlot
	static void maintainSyncPeriodFromMaintainSyncPeriod();
	static void startSyncSlotWithoutScheduledPrelude();
	static void syncSlotAfterNullSyncSlot();
	static void syncSlotAfterSyncSlot();
	static void syncSlotAfterFishSlot();
	static void syncSlotAfterMerging();
	static void radioPreludeTaskWSync();
	static void startSyncSlotFromPreludeStart();

	// Waypoints of SyncSlot
	static void syncSendTask();
	static void syncSlotEndFromSend();
	static void syncSlotEndFromListen();

	// to slots in normally sleeping slots
	static void omitNonsyncSlot();
	static void fishing();
	static void radioPreludeTaskWFish();
	static void startFishSlotFromPreludeStart();
	static void fishSlotStart();
	static void fishSlotEnd();

	static void provisionStart();
	static void merger();
};
