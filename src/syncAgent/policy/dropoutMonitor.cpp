#include <cassert>

#include "policyParameters.h"
#include "dropoutMonitor.h"


namespace {

ScheduleCount countSyncSlotsWithoutSyncMsg;

} // namespace


/*
 * constructor and heardSync() have same effect: reset counter
 */

void DropoutMonitor::reset() { countSyncSlotsWithoutSyncMsg = 0; }

void DropoutMonitor::heardSync() { reset(); }

bool DropoutMonitor::isDropout(){
	assert(countSyncSlotsWithoutSyncMsg <= Policy::maxMissingSyncsPerDropout);

	countSyncSlotsWithoutSyncMsg++;
	bool result = countSyncSlotsWithoutSyncMsg > Policy::maxMissingSyncsPerDropout;
	if (result) reset();
	return result;
}
