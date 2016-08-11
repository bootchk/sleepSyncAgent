
#include <cassert>
#include "syncAgent.h"
//#include "os.h"


// Static data members
bool SyncAgent::isSynching = false;
Clique SyncAgent::clique;
PowerManager* SyncAgent::powerMgr;
SyncPolicy SyncAgent::syncPolicy;
void (*SyncAgent::onSyncLostCallback)();


SyncAgent::SyncAgent(
		PowerManager* aPowerMgr,
		void (*aOnSyncLostCallback)()
	) {
	powerMgr = aPowerMgr;
	onSyncLostCallback = aOnSyncLostCallback;
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





