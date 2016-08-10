
#include "syncAgent.h"


// Static data members
Clique SyncAgent::clique;


// Temp standin for OS
static void scheduleTask(void callback() ) {
	
}

void SyncAgent::startSyncing() {
	clique.reset();
	// self is master of clique with no slaves

	// app schedule task
	scheduleSyncWake();

	// caller is sleeping, self will wake
}


 void SyncAgent::scheduleSyncWake() {
	scheduleTask(onSyncWake);
}


void SyncAgent::onSyncWake() {

}

