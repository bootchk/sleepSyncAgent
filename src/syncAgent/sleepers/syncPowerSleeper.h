
#include <nRF5x.h>	// Sleeper

/*
 * Knows how to sleep until power is adequate for sync agent.
 *
 * Shares a timer with SyncSleeper.
 */




class SyncPowerSleeper {
public:

	/*
	 * Sleep, waking periodically, until power is sufficient for SyncAgent to start.
	 *
	 * This should only be called when radio not in use and before SyncAgent is called.
	 */
	static void sleepUntilSyncPower();
};
