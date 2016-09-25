
#include <cassert>
#include "syncAgent.h"

#include "../platform/radio.h"	// xmit()


// Static data members
bool SyncAgent::isSyncing = false;
PowerManager* SyncAgent::powerMgr;
//OBS void (*SyncAgent::onSyncingPausedCallback)();
//OBS void (*SyncAgent::onWorkMsgCallback)(WorkMessage* msg);

Clique SyncAgent::clique;
DropoutMonitor SyncAgent::dropoutMonitor;
CliqueMerger SyncAgent::cliqueMerger;
Role SyncAgent::role;
SyncMessage SyncAgent::outwardSyncMsg;
WorkMessage SyncAgent::workMsg;
Serializer SyncAgent::serializer;


// This file only implements part of the class, see many other .cpp files.
// Start at syncAgentLoop.cpp for high level algorithm.

void SyncAgent::init(
		PowerManager* aPowerMgr,
		void (*aOnWorkMsgCallback)()
	) {
	powerMgr = aPowerMgr;
	//OBSonWorkMsgCallback = aOnWorkMsgCallback;

	clique.reset();
	// ensure initial state of SyncAgent
	assert(role.isFisher());
	assert(clique.isSelfMaster());
	// assert no tasks scheduled until startSyncing()
}


void SyncAgent::xmitSync(SyncMessage& msg) {
	xmit(serializer.serialize(msg));
}

void SyncAgent::xmitWork(WorkMessage& msg) {
	xmit(serializer.serialize(msg));
}


#ifdef OBS
void SyncAgent::startSyncing() {

	assert(! isSyncing);
	// Assert never had sync, or lost sync

	// Alternative: try recovering lost sync
	// Here, brute force: start my own clique

	clique.reset();
	assert(clique.isSelfMaster());
	// clique schedule starts now, at time of this call.
	// clique is not in sync with others, except by chance.

	// OBS scheduleSyncWake();

	// OBS calling app can sleep, wake event onSynchWake()
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
#endif





