
#include "dropoutMonitor.h"

int DropoutMonitor::countSyncSlotsWithoutSyncMsg;

void DropoutMonitor::heardSync() {
	countSyncSlotsWithoutSyncMsg = 0;
};

bool DropoutMonitor::check(){
	countSyncSlotsWithoutSyncMsg++;
	bool result = countSyncSlotsWithoutSyncMsg > 6;	// TODO symbolic
	return result;
}
