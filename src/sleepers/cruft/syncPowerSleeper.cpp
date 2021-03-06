#include <cassert>

#include "syncPowerSleeper.h"

#include "../scheduleParameters.h"
#include "../modules/syncPowerManager.h"
#include "syncSleeperObs.h"


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
	/*
	 * Design discussion:
	 * Could be a while()do{} instead.
	 * But safer to always sleep some first: it more often guarantees more power.
	 * Especially if you tighten power levels or if PowerManager is inaccurate.
	 */
	do {
		// assertUltraLowPower();	// Even this assert may consume more power than we have?

		/*
		 * !!! Must sleep entire timeout duration.
		 */
		//SyncSleeper::sleepUntilTimeout(ScheduleParameters::TimeoutWaitingForSyncPowerSleeper);
		Sleeper::sleepDuration(ScheduleParameters::TimeoutWaitingForSyncPowerSleeper);

		/*
		 * Not expected to wake for CounterOverflow and unexpected events.
		 * Check that we slept full timeout.
		 */
		ReasonForWake reason = Sleeper::getReasonForWake();
		assert(reason==ReasonForWake::SleepTimerExpired);
	}
	while (!SyncPowerManager::isPowerForStartLoop());

}
