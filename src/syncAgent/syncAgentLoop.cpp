

#include <cassert>
#include "syncAgent.h"

/*
SyncAgent is a task(thread) that infinite loops.

Define high level flow: a repeating sequence of slots.
*/

void SyncAgent::loop(){
	assert(clique.isSelfMaster());
	//startSyncing();
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
			sleepUntilTimeout();
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
			clique.schedule.timeTilThisSyncSlotEnd);
	//waitForMsgOrTimeout(timeToSyncSlotEnd);
	endSyncSlot();

	// work slot follows sync slot with no delay
	startWorkSlot();
	// radio on
	//waitForMsgOrTimeout(timeToWorkSlotEnd);
	dispatchMsgUntil(
			dispatchMsgReceivedInSyncSlot,
			clique.schedule.timeTilThisSyncSlotEnd);
	endWorkSlot();

	// Variation: next event if any occurs within a large sleeping time (lots of 'slots')
	if (role.isMerger()) {
		// avoid collision
		if (cliqueMerger.shouldScheduleMerge())  {

			sleepUntilTimeout();	//scheduleMergeWake();
			startMergeSlot();	// doMerge
			// Merge is xmit only, no sleeping til end of slot
		}
		// else sleep until end of sync period
	}
	else {
		// A fish slot need not be aligned with other slots, and different duration???
		sleepUntilTimeout(); // Fish every period       scheduleFishWake();
		startFishSlot();
		dispatchMsgUntil(
				dispatchMsgReceivedInSyncSlot,
				clique.schedule.timeTilThisSyncSlotEnd);
		endFishSlot();
	}
	//sleepTilNextSlot(nextSlotEnd);
	// radio off
	sleepUntilTimeout();
	// Period over
}
