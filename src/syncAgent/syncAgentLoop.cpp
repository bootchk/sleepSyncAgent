

#include <cassert>
#include "syncAgent.h"

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

	assert(! isSyncingState);
	while (true){
		// Sync period is either active or idle, but still advances schedule
		clique.schedule.startPeriod();

		if ( powerMgr.isPowerForRadio() ) {
			// FUTURE if !isSyncingState resumeSyncing  announce to app
			isSyncingState = true;
			doSyncPeriod();
		}
		else {
			if (isSyncingState) { pauseSyncing(); }
			isSyncingState = false;
			assert(!radio->isPowerOn());
			sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaNowToNextSyncPeriod());
			// sleep an entire sync period, then check power again.
		}
	}
	// never returns
}

/*
 * Active sync period is structured sequence of slots, some varying
 */
void SyncAgent::doSyncPeriod() {

	startSyncSlot();
	// FUTURE: xmit sync in middle of sync slot instead of at start???
	assert(!radio->isDisabled()); // listening for other's sync
	dispatchMsgUntil(
			dispatchMsgReceivedInSyncSlot,
			clique.schedule.deltaToThisSyncSlotEnd);
	endSyncSlot();

	// work slot follows sync slot with no delay
	startWorkSlot();
	assert(!radio->isDisabled());   // listening for other's work
	dispatchMsgUntil(
			dispatchMsgReceivedInSyncSlot,
			clique.schedule.deltaToThisWorkSlotEnd);
	endWorkSlot();
	assert(!radio->isPowerOn());	// Low power until next slot

	// Variation: next event (if any) occurs within a large sleeping time (lots of 'slots')
	if (role.isMerger()) {
		// avoid collision
		if (cliqueMerger.shouldScheduleMerge())  {

			sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisMergeStart(cliqueMerger.offsetToMergee));
			startMergeSlot();	// doMerge
			// Merge is xmit only, no sleeping til end of slot
		}
		// else sleep until end of sync period
	}
	else {
		// Fish every period
		// FUTURE: A fish slot need not be aligned with other slots, and different duration???
		sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisFishSlotStart());
		startFishSlot();
		dispatchMsgUntil(
				dispatchMsgReceivedInSyncSlot,
				clique.schedule.deltaToThisFishSlotEnd);
		endFishSlot();
	}
	assert(!radio->isPowerOn());	// Low power for remainder of this sync period
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaNowToNextSyncPeriod());
	// Sync period over
}
