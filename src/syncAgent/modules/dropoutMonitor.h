
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

public:
	DropoutMonitor() { reset(); }
	static void heardSync();
	static bool check();

private:
	static void reset() { countSyncSlotsWithoutSyncMsg = 0; }
};


