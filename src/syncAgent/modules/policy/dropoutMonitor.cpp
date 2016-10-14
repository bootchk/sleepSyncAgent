#include <cassert>

#include "policyParameters.h"
#include "dropoutMonitor.h"

ScheduleCount DropoutMonitor::countSyncSlotsWithoutSyncMsg;


void DropoutMonitor::heardSync() { reset(); }

bool DropoutMonitor::check(){
	assert(countSyncSlotsWithoutSyncMsg <= Policy::maxMissingSyncsPerDropout);	// was reset()

	countSyncSlotsWithoutSyncMsg++;
	bool result = countSyncSlotsWithoutSyncMsg > Policy::maxMissingSyncsPerDropout;
	return result;
}
