

#include <cassert>

#include "globals.h"	// radio, etc.
#include "syncAgent.h"

#include "modules/syncState.h"
#include "logMessage.h"
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

SyncState syncState;


#ifdef SIMPLE_SYNC_PERIOD
SimpleSyncPeriod syncPeriod;
#else
CombinedSyncPeriod syncPeriod;
#endif


void sleepEntireSyncPeriod() {
	syncSleeper.sleepUntilTimeout(clique.schedule.deltaNowToNextSyncPoint);
}









} // namespace


// FUTURE, we could check power again before each slot, namely fishing slot


void SyncAgent::loop(){
	/*
	 * Assertions on enter loop:
	 * - self is master of its own clique
	 * - not in sync yet
	 * - radio not in use
	 * - longClock is running but might not be accurate until LFXO is stable
	 * - there was power for the radio before we called this (since may have been exhausted by cpu execution.)
	 */
	assert(clique.isSelfMaster());
	assert(network.isLowPower());

	// DEBUG
	initLogging();
	log("ID:\n");
	logLongLong(clique.getMasterID());

	/*
	 * assert schedule already started and not too much time has elapsed
	 * Note that we roll forward at the end of the loop.
	 * If we roll forward at the beginning of the loop,
	 * we need to initialize schedule differently.
	 */

	while (true){

		onSyncPointCallback();	// call back app

		// app may have queued work

		workManager.resetState();

		assert(network.isLowPower());	// After every sync period

		if ( powerManager->isPowerForSync() ) {
			/*
			 * Sync keeping: enough power to use radio for two slots
			 */
			syncState.setActive();
			// SyncPeriod also checks power
			syncPeriod.doSlotSequence();
		}
		else {
			/*
			 * Sync maintenance: keep schedule by sleeping one sync period, w/o using radio
			 */
			syncState.setPaused();	// side effects
			sleepEntireSyncPeriod();
			// continue to check power for radio.
			// We may exhaust it and brown out, losing sync altogether
		}
		// Sync period over, advance schedule.
		// Keep schedule even if not enough power to listen/send sync messages to maintain accuracy
		clique.schedule.rollPeriodForwardToNow();
	}
	// never returns
}

#ifdef OLD
namespace {

/*
 * Sleep (not spin!!!) to recover boot energy and to insure LFXO is stable (takes 0.25 seconds.)
 * Necessary when power is load switched and hysteresis is low (.05V) and power storage is small capacitor.
 */
void waitForOSClockAndToRecoverBootEnergy(LongClockTimer * aLCT) {
	// Init sleeper with a larger timeout limit than while syncing
	syncSleeper.init(
			ScheduleParameters::StabilizedClockTimeout + 1,
			aLCT);

	syncSleeper.sleepUntilTimeout(ScheduleParameters::StabilizedClockTimeout);
}



}
#endif
