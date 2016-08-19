
// Copyright   : 2016, lloyd konneker

#include "powerManager.h"
#include "syncAgent/syncAgent.h"
#include "syncAgent/message.h"


// Callbacks from SyncAgent

void onSyncLost() {
	// SyncAgent determined power inadequate to continue sync.
	// TODO schedule a wakeTask to monitor power
}

void onWorkMsg(SyncMessage msg) {
	// SyncAgent received work msg.
	// TODO schedule low priority task to do work
	// realtime constrained
}


PowerManager powerMgr;
SyncAgent syncAgent(&powerMgr, onSyncLost, onWorkMsg);


void onPowerRestored() {
	syncAgent.resumeAfterPowerRestored();
}


int main() {
	//App app;
	
	//syncAgent.setTaskScheduler(scheduleSyncTask);
	syncAgent.startSyncing();
	
	// cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}

/*

Test:
without:
- an os to schedule
- a radio
- a second unit

create dummy messages and call a normal
sequence of SyncAgent callbacks.
syncAgent.onSyncWake();
syncAgent.onMsgReceived();
syncAgent.onSyncEnd();
// work msg
syncAgent.onWorkEnd();
etc. fish and merge


*/
