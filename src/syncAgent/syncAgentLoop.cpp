

#include <cassert>

#include "globals.h"	// PowerManager, etc.
#include "syncAgent.h"

#include "syncPeriod/syncPeriod.h"



/*
 * SyncAgent is a task(thread) that is infinite sequence of sync periods.
 *
 * Sync periods are active if enough power.
 * In inactive sync periods, schedule advances.
 * After enough inactive sync periods, schedule is much drifted.
 * After inactive sync periods, we attempt to resume drifted schedule.
*/

namespace {

#ifdef SIMPLE_SYNC_PERIOD
SimpleSyncPeriod syncPeriod;
#else
CombinedSyncPeriod syncPeriod;
#endif

// FUTURE, we could check power again before each slot, namely fishing slot
PowerManager powerManager;

} // namespace



void SyncAgent::loop(){
	// When first enter loop, each unit is master of its own clique
	assert(clique.isSelfMaster());

	// DEBUG
	initLogging();
	log("ID:\n");
	logLongLong(clique.getMasterID());

	assert(! isSyncingState);
	assert(!radio->isPowerOn());

	/*
	 * assert schedule already started and not too much time has elapsed
	 * Note that we roll forward at the end of the loop.
	 * If we roll forward at the beginning of the loop,
	 * we need to initialize schedule differently.
	 */

	while (true){
		// call back app
		onSyncPointCallback();

		assert(!radio->isPowerOn());	// Radio is off after every sync period

		if ( powerManager.isPowerForRadio() ) {
			/*
			 * Sync keeping: use radio
			 */
			// FUTURE if !isSyncingState resumeSyncing  announce to app
			isSyncingState = true;
			syncPeriod.doSlotSequence();
		}
		else {
			/*
			 * Sync maintenance: don't use radio but keep schedule by sleeping one sync period.
			 */
			if (isSyncingState) { pauseSyncing(); }
			isSyncingState = false;
			syncSleeper.sleepUntilTimeout(clique.schedule.deltaNowToNextSyncPoint);
			// sleep an entire sync period, then check power again.
		}
		// Sync period over, advance schedule.
		// Keep schedule even if not enough power to xmit sync messages to maintain accuracy
		clique.schedule.rollPeriodForwardToNow();
	}
	// never returns
}

