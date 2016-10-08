
#include "policyParameters.h"
#include "dropoutMonitor.h"

ScheduleCount DropoutMonitor::countSyncSlotsWithoutSyncMsg;


void DropoutMonitor::heardSync() { reset(); }

bool DropoutMonitor::check(){
	countSyncSlotsWithoutSyncMsg++;
	bool result = countSyncSlotsWithoutSyncMsg > Policy::maxMissingSyncsPerDropout;
	return result;
}
