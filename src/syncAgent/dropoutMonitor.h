
#pragma once

#include "../types.h"

// Monitor loss of sync: too many sync slots without hearing sync
class DropoutMonitor {

public:

	DropoutMonitor() {
		countSyncSlotsWithoutSyncMsg = 0;
	}
	static void heardSync();
	static bool check();

private:
	static ScheduleCount countSyncSlotsWithoutSyncMsg;

};


