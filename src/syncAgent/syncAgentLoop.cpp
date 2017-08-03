

#include <cassert>

#include "globals.h"	// clique
#include "syncAgent.h"
#include "scheduleParameters.h"

#include "state/syncState.h"
#include "syncPeriod/syncPeriod.h"
#include "sleepers/syncSleeper.h"
#include "state/phase.h"

#include "policy/workManager.h"

#include "logging/logger.h"

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
	Phase::set(PhaseEnum::SleepEntireSyncPeriod);
	SyncSleeper::sleepUntilTimeout(clique.schedule.deltaNowToNextSyncPoint);
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
	assert(!Ensemble::isRadioInUse());

	// DEBUG
	Logger::init();

	Logger::log(clique.getMasterID());
	Logger::log("<ID\n");

	// Set sane timeout for SyncPeriod calculations, different from that used by SyncPowerSleeper
	Sleeper::setSaneTimeout( ScheduleParameters::MaxSaneTimeoutSyncSleeper );

	// We expect not to brownout henceforth
	SyncPowerManager::enterBrownoutDetectMode();
	// Detection not in force until first call to measure power


	/*
	 * assert schedule already started.
	 * But some time has elapsed.
	 * Roll forward at the beginning of the loop.
	 *
	 * Rolling forward can have different implementations.
	 * At least the very first call must ensure that startTimeOfPeriod == nowTime();
	 */

	while (true){

		/*
		 * Sync period over,
		 * Assert now == timeAtStartOfSyncPeriod + syncPeriodDuration
		 * i.e. we must have slept the proper duration.
		 * Advance schedule, even if not enough power to listen/send sync messages to maintain accuracy.
		 */
		clique.schedule.rollPeriodForward();

		assert(Ensemble::isLowPower());	// After every sync period

		// TODO move this to where it could not possible interfere with SyncSlot
		Phase::set(PhaseEnum::SyncPointCallback);
		onSyncPointCallback();	// call back app, which must return quickly

		// app may have queued work

		WorkManager::resetState();

		if ( SyncPowerManager::isPowerForSync() ) {
			/*
			 * Sync keeping: enough power to use radio for two slots
			 */
			syncState.setActive();
			// SyncPeriod also checks power
			syncPeriod.doSlotSequence();
		}
		else {
			/*
			 * Paused state:
			 * Approximate sync schedule by marking sync periods, w/o using radio.
			 *
			 * Especially when starting up, and depending on how power levels are configured,
			 * and how much energy is harvested:
			 * Might be in this state for just one or two sync periods,
			 * and even in active sync keeping, we don't xmit sync every period anyway.
			 */

			syncState.setPaused();	// side effects limited to syncingState

			/*
			 * Are we paused for long time and are we master?
			 */

			if ( SyncState::shouldAbandonMastership() ) {
				// doAbandonMastershipSyncPeriod
				// For now, no abandon mastership i.e. we never get here
				assert(false);
			}
			else {
				sleepEntireSyncPeriod();
			}
			// continue to check power for radio.
			// We may exhaust it and brown out, losing sync altogether
		}

		// SyncPeriod over and next one starts.
	}
	// never returns
}

