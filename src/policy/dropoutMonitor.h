
#pragma once


/*
 * Dropout: the condition of failing to hear sync for a long time.
 * Master could be powered off or moved away or just too much contention.
 *
 * Class monitors the condition.
 *
 * Pure class, no instances
 */

class DropoutMonitor {

public:
	static void reset();

	static void heardSync();

	static bool isDropout();
};


