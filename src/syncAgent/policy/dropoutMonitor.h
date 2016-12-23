
#pragma once


/*
 * Dropout: the condition of failing to hear sync for a long time.
 * Master could be powered off or moved away or just too much contention.
 *
 * Class monitors the condition.
 */

class DropoutMonitor {

public:
	DropoutMonitor();

	static void heardSync();

	static bool isDropout();
};


