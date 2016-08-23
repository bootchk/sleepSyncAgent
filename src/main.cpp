
/*
 * Main for app using SleepSyncAgent.
 *
 * Deal with RTOS:
 * - tasks
 * - queues
 * - standard boilerplate
 *
 * Copyright 2016 lloyd konneker
 */


#include "powerManager/powerManager.h"
#include "syncAgent/syncAgent.h"
#include "syncAgent/message.h"


// TODO work thread
void onWorkMsg(WorkMessage* msg) {
	// SyncAgent received work msg.
	// TODO schedule low priority task to do work
	// realtime constrained
}


PowerManager powerMgr;
SyncAgent syncAgent(&powerMgr, onWorkMsg);

/*OBS
void onPowerRestored() {
	syncAgent.resumeAfterPowerRestored();
}
*/

int main() {
	syncAgent.loop();	// never returns
	
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
