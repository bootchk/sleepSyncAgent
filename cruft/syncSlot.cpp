#ifdef OBS

Old design

void SyncAgent::onSyncWake() {
	// sync slot starts
	if ( !powerMgr->isPowerForRadio() ) {
		pauseSyncing();
		// assert App has a scheduled event; SyncAgent has no events scheduled
	}
	else {
		startSyncSlot();
		// assert receiver on and endSyncSlotTask is scheduled
	}

	// ensure endSyncSlotTask is scheduled or App wakingTask is scheduled
	// sleep
}

// Scheduling


void SyncAgent::scheduleNextSyncRelatedTask() {
	// assert in syncSlot
	if (role.isMerger()) {
		// avoid collision
		if (cliqueMerger.shouldScheduleMerge()) {
			scheduleMergeWake();
		}
		else { scheduleSyncWake(); }
	}
	else {
		// Fish every period
		scheduleFishWake();
	}
	// assert some task scheduled
	// onSyncWake: start of next period
	// onMergeWake or onFishWake: in a normally-sleeping slot of this period
}


void SyncAgent::scheduleSyncWake() {
	// assert in work or fish or merge slot
	clique.schedule.scheduleStartSyncSlotTask(onSyncWake);
}



void SyncAgent::scheduleFishWake(){
	// assert in workSlot
	/*
	 * Schedule for random sleeping slot.
	 * Not to avoid collision of xmits, since fishing is receiving.
	 * Random to better cover time.
	 */
	clique.schedule.scheduleStartFishSlotTask(onFishWake);
}

void SyncAgent::scheduleMergeWake(){
	// Knows how to schedule mergeSlot at some time in current period
	// assert we have decided to send a mergeSync
	assert(cliqueMerger.isActive);
	clique.schedule.scheduleStartMergeSlotTask(onMergeWake, cliqueMerger.offsetToMergee);
}

void SyncAgent::startSyncSlot() {
	// Start of sync slot coincident with start of period.
	clique.schedule.startPeriod();

	xmitRoleAproposSync();

	// even a Master listens for remainder of sync slot
	turnReceiverOnWithCallback(onMsgReceivedInSyncSlot);
	clique.schedule.scheduleEndSyncSlotTask(onSyncSlotEnd);
    // assert radio on
	// will wake on onMsgReceivedInSyncSlot or onSyncSlotEnd
	// sleep



void SyncAgent::onSyncSlotEnd() {
	/*
	 * This may be late, when message receive thread this delays this.
	 * Also, there could be a race to deliver message with this event.
	 * FUTURE check for those cases.
	 * Scheduling of subsequent events does not depend on timely this event.
	 */

	// FUTURE we could do this elsewhere, e.g. start of sync slot
	if (dropoutMonitor.check()) {
		dropoutMonitor.heardSync();	// reset
		clique.onMasterDropout();
	}

	scheduleNextSyncRelatedTask();
	// workSlot follows syncSlot.  Fall into it.
	startWorkSlot();
	/*
	 * Assert onWorkSlotEnd scheduled
	 * AND some sync-related task is scheduled.
	 *
	 * assert radio on for work msgs
	 */
	// sleep
}

#endif
