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
#include "syncAgent.h"

PowerManager powerMgr;

void onSyncLost() {
	// Called by SyncAgent when power inadequate to continue sync
	// TODO schedule a wakeTask to monitor power
}


SyncAgent syncAgent(&powerMgr, onSyncLost);

/*
 void scheduleSyncTask(){
	// Schedule a task with callback to 
	// TBD schedule task
	// TBD separate function ... casting problems
	syncAgent.onSyncWake();
}
*/

int main() {
	//App app;
	
	//syncAgent.setTaskScheduler(scheduleSyncTask);
	syncAgent.startSyncing();
	
	// cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
