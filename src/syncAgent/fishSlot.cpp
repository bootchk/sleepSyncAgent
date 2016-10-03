/*
 * SyncAgent methods used during THE fishSlot of my schedule.
 * See general notes at syncSlot.
 *
 * Every unit can fish (if not busy merging.)
 * Fish for other cliques by listening.
 */

#include <cassert>

#include "syncAgent.h"


void SyncAgent::doFishSlot() {
	// FUTURE: A fish slot need not be aligned with other slots, and different duration???

	// Sleep ultra low-power across normally sleeping slots to start of fish slot
	assert(!radio->isPowerOn());
	sleeper.sleepUntilEventWithTimeout(clique.schedule.deltaToThisFishSlotStart());

	// TODO calulate timeout before starting receiver, take out of race
	startFishSlot();
	dispatchMsgUntil(
			dispatchMsgReceivedInSyncSlot,
			clique.schedule.deltaToThisFishSlotEnd);
	endFishSlot();
}


void SyncAgent::startFishSlot() {
	radio->powerOnAndConfigure();
	sleeper.clearReasonForWake();
	radio->receiveStatic();		// DYNAMIC receiveBuffer, Radio::MaxMsgLength);
	// assert can receive an event that wakes imminently: race to sleep
}


void SyncAgent::endFishSlot(){
	/*
	 * Conditions:
	 * (no sync msg was heard and receiver still on)
	 * OR (heard a sync msg and (adjusted my schedule OR changed role to Merger))
	 *
	 * In case we adjusted my schedule, now is near its beginning.
	 * The next scheduled sync slot will be almost one full period from now.
	 *
	 * In case we adjusted changed role to Merger,
	 * first mergeSlot will be after next syncSlot.
	 */
	// not require receiver on
	radio->stopReceive();
	radio->powerOff();
}


void SyncAgent::doSyncMsgInFishSlot(SyncMessage* msg){
	if (msg->isOffsetSync()) {
		// Ignore: other clique is already merging
	}
	else {
		toMergerRole(msg);
	}
	// Assert msg ignored or isMergerRole
}



/*
 * FUTURE act on it even though we are out of sync
void SyncAgent::doWorkMsgInFishSlot(SyncMessage msg) {
	// Relay to app
	onWorkMsgCallback(msg);
}
*/

