
#include "syncPowerSleeper.h"
#include "globals.h"	// powerManager, sleeper
#include "scheduleParameters.h"


/*
 * Knows how to sleep until power is adequate for sync agent.
 *
 * This shares a timer with SyncSleeper.
 */



/*
 * Sleep, waking periodically, until power is sufficient for radio.
 *
 * This should only be called when radio not in use
 * and before SyncAgent is called.
 *
 * Sleeper uses a callback which also sets ReasonForWake, but we ignore that.
 */
void SyncPowerSleeper::sleepUntilSyncPower(){
	while (!powerManager->isPowerForSync()){

		// not assertUltraLowPower() but it should be
		sleeper.sleepUntilEventWithTimeout(ScheduleParameters::MaxSaneTimeoutSyncPowerSleeper);
		// May wake for CounterOverflow and unexpected events.
	}

}
