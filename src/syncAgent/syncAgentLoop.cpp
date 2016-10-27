

#include <cassert>
#include "../platform/platform.h"
#include "syncAgent.h"
#include "globals.h"



/*
 * SyncAgent is a task(thread) that infinite sequence of sync periods.
 *
 * Sync periods are active if enough power.
 * In inactive sync periods, schedule advances.
 * After enough inactive sync periods, schedule is much drifted.
 * After inactive sync periods, we attempt to resume drifted schedule.
*/



void SyncAgent::loop(){
	// When first enter loop, each unit is master of its own clique
	assert(clique.isSelfMaster());

	ledLogger.init();	// DEBUG
	initLogging();

	log("Master ID: \n");
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
		// Sync period over, advance schedule.
		// Keep schedule even if not enough power to xmit sync messages to maintain accuracy
		clique.schedule.rollPeriodForwardToNow();
	}
	// never returns
}

/*
 * Active sync period is structured sequence of slots, some varying
 */
void SyncAgent::doSyncPeriod() {

	// syncSlot first, always
	syncSlot.perform();

	#ifdef FUTURE
	// Variation: work slot may be merging
	if (role.isWorkMerger()) {
		workSlot.performWorkMerger();
	}
	else {
		workSlot.performWork();
	}
#endif

	workSlot.performWork();

	assert(!radio->isPowerOn());	// Low power until next slot

	// Variation: next event (if any) occurs within a large sleeping time (lots of 'slots')
	if (role.isMerger()) {
		// avoid collision
		if (mergeSlot.mergePolicy.shouldScheduleMerge())  {
			mergeSlot.perform();
			// We might have quit role Merger
		}
		// else continue and sleep until end of sync period
	}
	else {
		// Fish every period
		fishSlot.perform();
		// continue and sleep until end of sync period
	}
	assert(!radio->isPowerOn());	// Low power for remainder of this sync period
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaNowToNextSyncPoint());
	// Sync period completed
}
