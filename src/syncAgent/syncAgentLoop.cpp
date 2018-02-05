

#include <cassert>

#include "../globals.h"	// clique
#include "syncAgent.h"
#include "../scheduleParameters.h"

#include "../modules/syncPowerManager.h"
#include "../syncPeriod/syncPeriod.h"

#include "state/syncMode.h"

#include "../sleepers/scheduleSleeper.h"
#include "state/phase.h"

#include "../policy/workManager.h"

#include "../slots/fishing/fishSchedule.h"	// logging

#include "../logging/logger.h"
#include "../logging/remoteLogger.h"

/*
 * SyncAgent is a task(thread) that is infinite sequence of sync periods.
 *
 * Sync periods are active (using radio) if enough power.
 * In inactive sync periods, schedule advances.
 * After enough inactive sync periods, schedule is much drifted.
 * After inactive sync periods, we try to resume drifted schedule.
 *
 * Active sync periods always do sync slot.
 * Sync periods may also fish when enough power.
*/

namespace {

// Obsolete alternative SimpleSyncPeriod

void doModalSyncPeriod() {
	switch(SyncModeManager::mode()) {
	case SyncMode::Maintain:
		ScheduleSleeper::sleepEntireSyncPeriod();
		break;

	/*
	 * CombinedSyncPeriod also checks FisherMergerRole (sub mode.)
	 */
	case SyncMode::SyncOnly:
		CombinedSyncPeriod::doSlotSequence();
		break;
	case SyncMode::SyncAndFishMerge:
		CombinedSyncPeriod::doSlotSequence();
		break;

	case SyncMode::SyncAndProvision:
#ifdef BLE_PROVISIONED
		ProvisioningSyncPeriod::doSlotSequence();
#else
		assert(false);	// illegal to be in this state when not built for provisioning
#endif
			break;
	}
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

	Logger::init();
	Logger::logSystemInfo();
	FishSchedule::logParameters();

	// Set sane timeout for SyncPeriod calculations, different from that used by SyncPowerSleeper
	Sleeper::setSaneTimeout( ScheduleParameters::MaxSaneTimeoutSyncSleeper );

	// We expect not to brownout henceforth
	SyncPowerManager::enterBrownoutDetectMode();
	// Detection not in force until first call to measure power

	/*
	 * This requires some power to transmit.
	 * Faults are in flash and this does not erase them.
	 * So if power is low, this might result in: Brownout, SoftReset, sendAnyFaults, Brownout, ...
	 */
	RemoteLogger::sendAnyFaults();

	SyncModeManager::init();
	// assert in Maintain mode and
	// assert(MergerFisherRole::role()==Role::Waiting);

	/*
	 * Assert schedule already started, but clock has ticked in meantime.
	 * The roll forward at the beginning of the loop is in fixed increment.
	 * Here we need to restart schedule to ensure that startTimeOfPeriod == nowTime();
	 */
	clique.schedule.setEndTimeOfSyncPeriodToNow();

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

		/*
		 * Remote logging is high priority.
		 * Do it regardless of mode and forego usual modal
		 */
		if (RemoteLogger::isEnabled() and RemoteLogger::trySendingLog()) {
			// InfoSlot was performed, sleep remainder of SyncPeriod
			ScheduleSleeper::sleepEntireSyncPeriod();
		}
		else {
			SyncModeManager::checkPowerAndTryModeTransitions();
			doModalSyncPeriod();
		}

		// SyncPeriod over and next one starts.
	}
	// never returns
}



