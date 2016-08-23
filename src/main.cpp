
/*
 * Main for app using SleepSyncAgent.
 *
 * Deal with RTOS:
 * - tasks
 * - queues
 * - standard boilerplate
 *
 * RTOS objects:
 *
 * Tasks/Threads, in order of priority
 * - hwi/ISR for timer for work (FUTURE)
 * - radioTask (stack provided by platform)
 * - syncTask (provided here)
 * - workTask
 *
 * Queues:
 * - msg queue from radioStack to syncTask
 * - msg queue from syncTask to radioStack
 * - msg queue from workTask to syncTask
 * - msg queue from syncTask to workTask
 *
 * Semaphores
 * - misc (provided by platform for msg queues, hidden e.g. by ICall)
 *
 * Priority
 * Here I assume work is lower priority than sync.
 * Sync is higher priority because work is not hard-realtime while sync messages are timing critical.
 * Power and priority are intertwined. I assume that work takes more power than sync.
 * Attempt to maintain sync even if there is not enough power to do work.
 * You can change that, you could even implement hysteresis:
 * important to achieve sync while power is rising,
 * but important to do work while power is falling but there is sync.
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
Compile:
there are stubs so it should compile without a platform implementation.

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

Two-unit test

Multiple unit test

*/
