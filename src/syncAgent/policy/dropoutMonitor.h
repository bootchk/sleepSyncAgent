
#pragma once


/*
 * Dropout: the condition of failing to hear sync for a long time.
 * Master could be powered off or moved away or just too much contention.
 *
 * Class monitors the condition.
 */
class DropoutMonitor {
private:
	static ScheduleCount countSyncSlotsWithoutSyncMsg;
	static void reset() { countSyncSlotsWithoutSyncMsg = 0; }

public:
	DropoutMonitor() { reset(); }

	/*
	 * heardSync() and reset() have same effect: reset counter
	 */
	static void heardSync();

	static bool isDropout();
};


