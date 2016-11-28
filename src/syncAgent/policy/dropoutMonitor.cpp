#include <cassert>

#include "policyParameters.h"
#include "dropoutMonitor.h"

ScheduleCount DropoutMonitor::countSyncSlotsWithoutSyncMsg;


void DropoutMonitor::heardSync() { reset(); }

bool DropoutMonitor::isDropout(){
	assert(countSyncSlotsWithoutSyncMsg <= Policy::maxMissingSyncsPerDropout);

	countSyncSlotsWithoutSyncMsg++;
	bool result = countSyncSlotsWithoutSyncMsg > Policy::maxMissingSyncsPerDropout;
	if (result) reset();
	return result;
}
