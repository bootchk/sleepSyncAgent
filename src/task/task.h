
#pragma once

/*
 * SleepSyncTask
 *
 * Knows the set of tasks for SleepSync.
 * Just a list of code fragments run at various waypoints of the SleepSync schedule.
 *
 * A "task" is run-to-completion and executes in ISR context.
 *
 * A "task" is not a class: no method common to all these tasks.
 * These methods ARE tasks, but they don't have class type "Task".
 *
 *
 */
class SSTask {
public:
	/*
	 * Startup task for primary SyncAgent operation.
	 */
	static void checkPowerReserve();

	static void radioPrelude();

	static void startSyncSlotAfterPrelude();

	static void startSyncSlotWithoutPrelude();

	// initiate send by syncSendKind (fork task on radio peripheral)
	static void sendSync();

	static void endSyncSlotListen();
	static void endSyncSlotSend();

	static void startSyncPeriodMaintain();
	static void startSyncSlotWithoutScheduledPrelude();

	static void tryFishOrMerge();


	static void fishSlotStart();
	static void fishSlotEnd();

	static void provisionStart();
	static void provisionEnd();

	static void mergerStartAndEnd();
	// FUTURE static void mergerEnd();
};
