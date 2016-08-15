//============================================================================
// Name        : sleepSyncAgent.cpp
// Author      : lloyd konneker
// Version     :
// Copyright   : 2016
// Description : 
//============================================================================

// #include <iostream>
// using namespace std;
//#include "app.h"
#include "powerManager.h"
#include "syncAgent/syncAgent.h"
#include "message.h"

PowerManager powerMgr;

void onSyncLost() {
	// Called by SyncAgent when power inadequate to continue sync
	// TODO schedule a wakeTask to monitor power
}

void onWorkMsg(Message msg) {
	// Called by SyncAgent when work msg received
	// TODO schedule low priority task to do work
	// realtime constrained
}


SyncAgent syncAgent(&powerMgr, onSyncLost, onWorkMsg);


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
