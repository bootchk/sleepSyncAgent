
#include <cassert>
#include "syncAgent.h"
//#include "os.h"


// Static data members
bool SyncAgent::isSynching = false;
PowerManager* SyncAgent::powerMgr;
void (*SyncAgent::onSyncLostCallback)();
void (*SyncAgent::onWorkMsgCallback)(Message msg);

Clique SyncAgent::clique;
Clique SyncAgent::otherClique;
DropoutMonitor SyncAgent::dropoutMonitor;
CliqueMerger SyncAgent::cliqueMerger;
Role SyncAgent::role;
Message SyncAgent::msg;




SyncAgent::SyncAgent(
		PowerManager* aPowerMgr,
		void (*aOnSyncLostCallback)(),
		void (*aOnWorkMsgCallback)(Message msg)
	) {
	powerMgr = aPowerMgr;
	onSyncLostCallback = aOnSyncLostCallback;
	onWorkMsgCallback = aOnWorkMsgCallback;

	// ensure initial state of SyncAgent
	assert(role.isFisher());
	assert(clique.isSelfMaster());
	// assert no tasks scheduled until startSyncing()
}

void SyncAgent::startSyncing() {
	// TODO don't use this flag; state is in the clique
	assert(! isSynching);
	// Assert never had sync, or lost sync

	// Alternative: try recovering lost sync
	// Here, brute force: start my own clique

	clique.reset();
	// self is master of clique with no slaves

	scheduleSyncWake();

	// ensure app scheduled onSyncWakeTask ow sleep forever
	// calling app should sleep, self will wake
}

void SyncAgent::resumeAfterPowerRestored() {
	/*
	 * Not reset clique.  Resume previous role and schedule.
	 * If little time has passed since lost power, might still be in sync.
	 * Otherwise self has drifted, and will experience masterDropout.
	 */
	clique.schedule.resumeAfterPowerRestored();
	scheduleSyncWake();
}





