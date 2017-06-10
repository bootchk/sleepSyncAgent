

#include <cassert>

#include "globals.h"	// radio, etc.
#include "syncAgent.h"
#include "scheduleParameters.h"

#include "state/syncState.h"
#include "logMessage.h"
#include "syncPeriod/syncPeriod.h"



/*
 * SyncAgent is a task(thread) that is infinite sequence of sync periods.
 *
 * Sync periods are active if enough power.
 * In inactive sync periods, schedule advances.
 * After enough inactive sync periods, schedule is much drifted.
 * After inactive sync periods, we try to resume drifted schedule.
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
	 * - radio power on
	 * - radio not in use
	 * - longClock is running but might not be accurate until LFXO is stable
	 * - there was power for the radio before we called this (since may have been exhausted by cpu execution.)
	 *
	 * Note not necessary to have PowerForSync before call, this will sleep until there is.
	 */
	assert(clique.isSelfMaster());
	assert(!network.isRadioInUse());

	// DEBUG
	initLogging();
	log("ID:\n");
	logLongLong(clique.getMasterID());

	// Set sane timeout for SyncPeriod calculations, different from that used by SyncPowerSleeper
	sleeper.setSaneTimeout( ScheduleParameters::MaxSaneTimeoutSyncSleeper );

	// We expect not to brownout henceforth
	syncPowerManager->enableBrownoutDetectMode();
	// Detection not in force until first call to measure power


	/*
	 * assert schedule already started and not too much time has elapsed
	 * Note that we roll forward at the end of the loop.
	 * If we roll forward at the beginning of the loop,
	 * we need to initialize schedule differently.
	 */

	while (true){

		phase = Phase::SyncPointCallback;
		onSyncPointCallback();	// call back app, which must return quickly

		// app may have queued work

		workManager.resetState();

		if ( syncPowerManager->isPowerForSync() ) {
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
			phase = Phase::SleepEntireSyncPeriod;
			sleepEntireSyncPeriod();
			// continue to check power for radio.
			// We may exhaust it and brown out, losing sync altogether
		}
		// Sync period over, advance schedule.
		// Keep schedule even if not enough power to listen/send sync messages to maintain accuracy
		clique.schedule.rollPeriodForwardToNow();

		assert(network.isLowPower());	// After every sync period
	}
	// never returns
}

