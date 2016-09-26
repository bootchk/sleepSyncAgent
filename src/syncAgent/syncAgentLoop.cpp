

#include <cassert>

#include "../platform/radio.h"
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
			// sleep an entire sync period, then check power again.
			if (isSyncing) {
				pauseSyncing();
			}
			isSyncing = false;
			// TODO symbolic constant, 1 is not right
			sleepUntilEventWithTimeout(1);
			// TODO assert radio off
		}
	}
}

/*
 * Active sync period is structured sequence of slots, some varying
 */
void SyncAgent::doSyncPeriod() {

	startSyncSlot();
	// TODO: xmit sync in middle of sync slot
	assert(isReceiverOn()); // listening for other's sync
	dispatchMsgUntil(
			dispatchMsgReceivedInSyncSlot,
			clique.schedule.deltaToThisSyncSlotEnd);
	endSyncSlot();

	// work slot follows sync slot with no delay
	startWorkSlot();
	assert(isReceiverOn());   // listening for other's work
	dispatchMsgUntil(
			dispatchMsgReceivedInSyncSlot,
			clique.schedule.deltaToThisWorkSlotEnd);
	endWorkSlot();
	assert(!isReceiverOn());	// Low power until next slot

	// Variation: next event (if any) occurs within a large sleeping time (lots of 'slots')
	if (role.isMerger()) {
		// avoid collision
		if (cliqueMerger.shouldScheduleMerge())  {

			sleepUntilEventWithTimeout(clique.schedule.deltaToThisMergeStart(cliqueMerger.offsetToMergee));
			startMergeSlot();	// doMerge
			// Merge is xmit only, no sleeping til end of slot
		}
		// else sleep until end of sync period
	}
	else {
		// Fish every period
		// FUTURE: A fish slot need not be aligned with other slots, and different duration???
		sleepUntilEventWithTimeout(clique.schedule.deltaToThisFishSlotStart());
		startFishSlot();
		dispatchMsgUntil(
				dispatchMsgReceivedInSyncSlot,
				clique.schedule.deltaToThisFishSlotEnd);
		endFishSlot();
	}
	assert(!isReceiverOn());	// Low power for remainder of this sync period
	assert(!isTransmitterOn());
	sleepUntilEventWithTimeout(clique.schedule.deltaNowToNextSyncPeriod());
	// Period over
}
