
#include "../syncAgentImp/syncAgentImp.h"

#include <cassert>

#include "../globals.h"	// clique
#include "../scheduleParameters.h"

#include "../modules/syncPowerManager.h"

#include "../work/workIn.h"

#include "../slots/fishing/fishSchedule.h"	// logging

#include "../logging/logger.h"
#include "../logging/remoteLogger.h"
#include "../syncAgentImp/state/phase.h"
#include "../syncAgentImp/state/syncMode.h"

#include "../schedule/syncSchedule.h"


/*
 * SyncAgentImp is a task(thread) that is infinite sequence of sync periods.
 *
 * Sync periods are active (using radio) if enough power.
 * In inactive sync periods, schedule advances.
 * After enough inactive sync periods, schedule is much drifted.
 * After inactive sync periods, we try to resume drifted schedule.
 *
 * Active sync periods always do sync slot.
 * Sync periods may also fish when enough power.
*/


// FUTURE, we could check power again before each slot, namely fishing slot

void SyncAgentImp::preludeToLoop() {
	assert(clique.isSelfMaster());
	assert(!Ensemble::isRadioInUse());

	Logger::init();
	Logger::logSystemInfo();
	FishSchedule::logParameters();

	// Obsolete
	// Set sane timeout for SyncPeriod calculations, different from that used by SyncPowerSleeper
	// Sleeper::setSaneTimeout( ScheduleParameters::MaxSaneTimeoutSyncSleeper );

	// We expect not to brownout henceforth
	SyncPowerManager::enterBrownoutDetectMode();
	// Detection not in force until first call to measure power

	/*
	 * This requires some power to transmit.
	 * Faults are in flash and this does not erase them.
	 * So if power is low, this might result in: Brownout, SoftReset, sendAnyFaults, Brownout, ...
	 */
	RemoteLogger::sendAnyFaults();

	SyncModeManager::resetToModeMaintain();
	// assert in Maintain mode and
	// assert(MergerFisherRole::role()==Role::Waiting);

	/*
	 * Assert schedule already started, but clock has ticked in meantime.
	 * The roll forward at the beginning of the loop is in fixed increment.
	 * Here we need to restart schedule to ensure that startTimeOfPeriod == nowTime();
	 */
	clique.schedule.setEndTimeOfSyncPeriodToNow();
}


void SyncAgentImp::callbackAppPreSync() {
	//Phase::set(PhaseEnum::SyncPointCallback);
	onSyncPointCallback();	// call back app, which must return quickly
}

void SyncAgentImp::preludeToSyncPeriod() {
	/*
	 * Sync period over,
	 * Assert now == timeAtStartOfSyncPeriod + syncPeriodDuration
	 * i.e. we must have slept the proper duration.
	 * Advance schedule, even if not enough power to listen/send sync messages to maintain accuracy.
	 */
	clique.schedule.rollPeriodForward();

	/*
	 * Earlier design, HFXO and radio off: Ensemble::isLowPower()
	 * Now, radio is off but HFXO might still be on.
	 */
	assert(not Ensemble::isRadioInUse());	// After every sync period

	// app may have queued work

	WorkIn::resetState();
}


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



#ifdef CRUFT
	/*
	 * RTC tasks design.
	 * Each task is RTC run-to-completion and is an ISR.
	 * Invariant: task is running, or some task scheduled.
	 * Main loop just sleeps, and all work is done in ISR's.
	 */
	SyncSchedule::initialSyncPeriod();
	while (true) {
		// MCUSleep::untilAnyEvent();
		MCUSleep::untilInterrupt();

		assertUltraLowPower();
	}
#endif

#ifdef OBSOLETE

#include <clock/mcuSleep.h>
#include "../sleepers/scheduleSleeper.h"

void SyncAgentImp::loop(){

	preludeToLoop();

	while (true){

		preludeToSyncPeriod();


		/*
		 * Remote logging is high priority.
		 * Do it regardless of mode and forego usual modal
		 */
		if (RemoteLogger::isEnabled() and RemoteLogger::trySendingLog()) {
			// InfoSlot was performed, sleep remainder of SyncPeriod
			ScheduleSleeper::sleepEntireSyncPeriod();
		}
		else {
			ModalSyncPeriod::perform();
		}

		// SyncPeriod over and next one starts.
	}
	// never returns
}

#endif


