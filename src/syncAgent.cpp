
#include "syncAgent.h"


// Static data members
Clique SyncAgent::clique;
PowerManager* SyncAgent::powerMgr;


// Temp standin for OS
static void scheduleTask(void callback() ) {
	
}


SyncAgent::SyncAgent(PowerManager* aPowerMgr) {
	powerMgr = aPowerMgr;
}

void SyncAgent::startSyncing() {
	clique.reset();
	// self is master of clique with no slaves

	// app schedule task
	scheduleSyncWake();

	// caller should sleep, self will wake
}


 void SyncAgent::scheduleSyncWake() {
	scheduleTask(onSyncWake);
}


void SyncAgent::onSyncWake() {
	// sync slot starts
	if ( !powerMgr->isPowerForRadio() ) {

	}

}

