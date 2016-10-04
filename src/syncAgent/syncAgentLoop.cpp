

#include <cassert>
#include "syncAgent.h"

#include "../platform/ledLogger.h"	// DEBUG

/*
 * SyncAgent is a task(thread) that infinite sequence of sync periods.
 *
 * Sync periods are active if enough power.
 * In inactive sync periods, schedule advances.
 * After enough inactive sync periods, schedule is much drifted.
 * After inactive sync periods, we attempt to resume drifted schedule.
*/

LEDLogger ledLogger;	// DEBUG


void SyncAgent::loop(){
	// When first enter loop, each unit is master of its own clique
	assert(clique.isSelfMaster());

	ledLogger.init();	// DEBUG

	assert(! isSyncingState);
	assert(!radio->isPowerOn());

	while (true){
		ledLogger.toggleLED(1);	// DEBUG, use the only LED on some targets

		assert(!radio->isPowerOn());	// Radio is off after every sync period

		// Sync period is either active or idle, but still advances schedule
		clique.schedule.rollPeriodForwardToNow();

		if ( powerMgr.isPowerForRadio() ) {
			// FUTURE if !isSyncingState resumeSyncing  announce to app
			isSyncingState = true;
			doSyncPeriod();
		}
		else {
			if (isSyncingState) { pauseSyncing(); }
			isSyncingState = false;
			sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaNowToNextSyncPoint());
			// sleep an entire sync period, then check power again.
		}
	}
	// never returns
}

/*
 * Active sync period is structured sequence of slots, some varying
 */
void SyncAgent::doSyncPeriod() {

	doSyncSlot();
	doWorkSlot();
	assert(!radio->isPowerOn());	// Low power until next slot

	// Variation: next event (if any) occurs within a large sleeping time (lots of 'slots')
	if (role.isMerger()) {
		// avoid collision
		if (cliqueMerger.shouldScheduleMerge())  {
			// TODO doMergeSlot
			sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisMergeStart(cliqueMerger.offsetToMergee));
			startMergeSlot();	// doMerge
			// Merge is xmit only, no sleeping til end of slot
		}
		// else continue and sleep until end of sync period
	}
	else {
		// Fish every period
		doFishSlot();
		// continue and sleep until end of sync period
	}
	assert(!radio->isPowerOn());	// Low power for remainder of this sync period
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaNowToNextSyncPoint());
	// Sync period completed
}
