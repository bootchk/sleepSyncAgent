

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
	assert(clique.isSelfMaster());
	//TODO startSyncing();
	while (true){
		// Sync period is either active or idle, but still advances schedule
		clique.schedule.startPeriod();

		if ( powerMgr->isPowerForRadio() ) {
			// TODO resumesyncing
			isSyncing = true;
			doSyncPeriod();
		}
		else {
			// sleep a sync period, then check power again.
			if (isSyncing) {
				pauseSyncing();
			}
			isSyncing = false;
			sleepUntilTimeout(1);
		}
	}
}

/*
 * Active sync period is structured sequence of slots, some varying
 */
void SyncAgent::doSyncPeriod() {

	startSyncSlot();
	// radio on, listening for other's sync
	dispatchMsgUntil(
			dispatchMsgReceivedInSyncSlot,
			clique.schedule.deltaToThisSyncSlotEnd);
	//waitForMsgOrTimeout(timeToSyncSlotEnd);
	endSyncSlot();

	// work slot follows sync slot with no delay
	startWorkSlot();
	// radio on
	//waitForMsgOrTimeout(timeToWorkSlotEnd);
	dispatchMsgUntil(
			dispatchMsgReceivedInSyncSlot,
			clique.schedule.deltaToThisSyncSlotEnd);
	endWorkSlot();

	// Variation: next event if any occurs within a large sleeping time (lots of 'slots')
	if (role.isMerger()) {
		// avoid collision
		if (cliqueMerger.shouldScheduleMerge())  {

			sleepUntilTimeout(clique.schedule.deltaToThisMergeStart(cliqueMerger.offsetToMergee));	//scheduleMergeWake();
			startMergeSlot();	// doMerge
			// Merge is xmit only, no sleeping til end of slot
		}
		// else sleep until end of sync period
	}
	else {
		// A fish slot need not be aligned with other slots, and different duration???
		sleepUntilTimeout(1); // Fish every period       scheduleFishWake();
		startFishSlot();
		dispatchMsgUntil(
				dispatchMsgReceivedInSyncSlot,
				clique.schedule.deltaToThisFishSlotEnd);
		endFishSlot();
	}
	//sleepTilNextSlot(nextSlotEnd);
	// radio off
	sleepUntilTimeout(1);
	// Period over
}
