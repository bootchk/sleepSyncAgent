

#include "masterXmitSyncPolicy.h"

ScheduleCount MasterXmitSyncPolicy::countDownToXmit;
ScheduleCount MasterXmitSyncPolicy::countDownToRollover;

void MasterXmitSyncPolicy::reset() {
	countDownToXmit = 0;
	countDownToRollover = 0;
}

// Called every sync slot
bool MasterXmitSyncPolicy::shouldXmitSync(){
	countDownToXmit--;
	countDownToRollover--;

	// Rollover
	if (countDownToRollover == 0) {
		countDownToRollover = 10;
	}
	bool result = countDownToXmit <= 0;
	return result;
}

