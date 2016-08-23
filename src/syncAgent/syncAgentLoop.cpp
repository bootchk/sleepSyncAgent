

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
	dispatchMsgUntil();

	//waitForMsgOrTimeout(timeToSyncSlotEnd);
	endSyncSlot();
	// work slot follows sync slot with no delay
	startWorkSlot();
	// radio on
	//waitForMsgOrTimeout(timeToWorkSlotEnd);
	dispatchMsgUntil();
	endWorkSlot();

	// Variation: next event if any is some varying time into a large sleeping time
	if (role.isMerger()) {
			// avoid collision
			if (cliqueMerger.shouldScheduleMerge())  sleepUntilTimeout();	//scheduleMergeWake();
			else sleepUntilTimeout(); // scheduleSyncWake(); }
		}
		else {
			sleepUntilTimeout(); // Fish every period       scheduleFishWake();
			startFishSlot();
			dispatchMsgUntil();
			endFishSlot();
		}
	//sleepTilNextSlot(nextSlotEnd);
	// radio off
	sleepUntilTimeout();
	// Period over
}
