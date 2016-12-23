#include <cassert>

#include "policyParameters.h"
#include "dropoutMonitor.h"


namespace {

ScheduleCount countSyncSlotsWithoutSyncMsg;

void reset() { countSyncSlotsWithoutSyncMsg = 0; }

} // namespace


/*
 * constructor and heardSync() have same effect: reset counter
 */

DropoutMonitor::DropoutMonitor() { reset(); }

void DropoutMonitor::heardSync() { reset(); }

bool DropoutMonitor::isDropout(){
	assert(countSyncSlotsWithoutSyncMsg <= Policy::maxMissingSyncsPerDropout);

	countSyncSlotsWithoutSyncMsg++;
	bool result = countSyncSlotsWithoutSyncMsg > Policy::maxMissingSyncsPerDropout;
	if (result) reset();
	return result;
}
