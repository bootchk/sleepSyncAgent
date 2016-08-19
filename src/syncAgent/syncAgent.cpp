
#include <cassert>
#include "syncAgent.h"
//#include "os.h"


// Static data members
bool SyncAgent::isPaused = false;
PowerManager* SyncAgent::powerMgr;
void (*SyncAgent::onSyncingPausedCallback)();
void (*SyncAgent::onWorkMsgCallback)(SyncMessage msg);

Clique SyncAgent::clique;
DropoutMonitor SyncAgent::dropoutMonitor;
CliqueMerger SyncAgent::cliqueMerger;
Role SyncAgent::role;
SyncMessage SyncAgent::msg;
WorkMessage SyncAgent::workMsg;




SyncAgent::SyncAgent(
		PowerManager* aPowerMgr,
		void (*aOnSyncingPausedCallback)(),
		void (*aOnWorkMsgCallback)(SyncMessage msg)
	) {
	powerMgr = aPowerMgr;
	onSyncingPausedCallback = aOnSyncingPausedCallback;
	onWorkMsgCallback = aOnWorkMsgCallback;

	// ensure initial state of SyncAgent
	assert(role.isFisher());
	assert(clique.isSelfMaster());
	// assert no tasks scheduled until startSyncing()
}

void SyncAgent::startSyncing() {

	assert(! isPaused);
	// Assert never had sync, or lost sync

	// Alternative: try recovering lost sync
	// Here, brute force: start my own clique

	clique.reset();
	assert(clique.isSelfMaster());
	// clique schedule starts now, at time of this call.
	// clique is not in sync with others, except by chance.

	scheduleSyncWake();

	// calling app can sleep, wake event onSynchWake()
}

void SyncAgent::resumeAfterPowerRestored() {
	/*
	 * Not reset clique.  Resume previous role and schedule.
	 * If little time has passed since lost power, might still be in sync.
	 * Otherwise self has drifted, and will experience masterDropout.
	 */
	assert(isPaused);
	isPaused = false;
	clique.schedule.resumeAfterPowerRestored();
	scheduleSyncWake();
}





