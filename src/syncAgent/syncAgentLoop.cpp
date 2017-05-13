

#include <cassert>

#include "globals.h"	// radio, etc.
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


void sleepEntireSyncPeriod() {
	syncSleeper.sleepUntilTimeout(clique.schedule.deltaNowToNextSyncPoint);
}


/*
 * Ask an other unit in my clique to assume mastership.
 * Might not be heard, in which case other units should detect DropOut.
 */
void doDyingBreath() {
	syncSender.sendAbandonMastership();
}


/*
 * Not enough power for self to continue syncing.
 * Other units might still have power and assume mastership of my clique
 */
void pauseSyncing() {
	assert(network.isLowPower());

	// FUTURE if clique is probably not empty
	if (clique.isSelfMaster()) doDyingBreath();
	// else I am a slave, just drop out of clique, others may have enough power

	// FUTURE onSyncingPausedCallback();	// Tell app
}

/*
 * We were not keeping sync, but about to start tying again (using radio.)
 */
void resumeSyncing() {
	// FUTURE onSyncingResumedCallback();	// Tell app
}

} // namespace


// FUTURE, we could check power again before each slot, namely fishing slot


void SyncAgent::loop(PowerManager* powerManager){
	// When first enter loop, each unit is master of its own clique
	assert(clique.isSelfMaster());

	// DEBUG
	initLogging();
	log("ID:\n");
	logLongLong(clique.getMasterID());

	assert(! isSyncingState);
	assert(network.isLowPower());

	/*
	 * assert schedule already started and not too much time has elapsed
	 * Note that we roll forward at the end of the loop.
	 * If we roll forward at the beginning of the loop,
	 * we need to initialize schedule differently.
	 */

	while (true){
		// call back app
		onSyncPointCallback();
		// app may have queued work

		workManager.resetState();

		assert(network.isLowPower());	// After every sync period

		if ( powerManager->isPowerForRadio() ) {
			/*
			 * Sync keeping: use radio
			 */
			if (!isSyncingState) {
				resumeSyncing();
				isSyncingState = true;
			}

			syncPeriod.doSlotSequence();
		}
		else {
			/*
			 * Sync maintenance: keep schedule by sleeping one sync period, w/o using radio
			 */
			if (isSyncingState) {
				pauseSyncing();
				isSyncingState = false;
			}

			sleepEntireSyncPeriod();
			// continue to check power.
		}
		// Sync period over, advance schedule.
		// Keep schedule even if not enough power to xmit sync messages to maintain accuracy
		clique.schedule.rollPeriodForwardToNow();
	}
	// never returns
}

