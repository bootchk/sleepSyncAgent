
#include "syncPowerSleeper.h"

#include "scheduleParameters.h"

#include "modules/syncPowerManager.h"


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
	while (!SyncPowerManager::isPowerForSync()){

		assertUltraLowPower();
		Sleeper::sleepUntilEventWithTimeout(ScheduleParameters::TimeoutWaitingForSyncPowerSleeper);
		// May wake for CounterOverflow and unexpected events.
	}

}
