
#include <cassert>

#include "../../../augment/random.h"
#include "../policyParameters.h"

#include "randomAlarmingClock.h"


/*
 * !!! Not a general implementation.
 * Uses constants that should be passed in the constructor.
 * Uses specific types from the application (SleepSync).
 */

namespace {

bool isAlarmEnabled;

// unsigned, counts upwards
ScheduleCount alarmTick;
ScheduleCount clockTick;

void setAlarm() {
	alarmTick = randUnsignedInt16(0, Policy::CountSyncPeriodsToChooseMasterSyncXmits-1);
	// alarmTick in [0, CountSyncPeriodsToChooseMasterSyncXmits-1]
}

} // namespace




void RandomAlarmingCircularClock::wrap() {
	clockTick = 0;
	setAlarm();
	isAlarmEnabled = true;
}


// Returns true if alarm goes off
bool RandomAlarmingCircularClock::tickWithAlarm(){
	// clockTick is unsigned => positive or zero
	assert(clockTick <= Policy::CountSyncPeriodsToChooseMasterSyncXmits-1);

	bool result;
	if (isAlarmEnabled) {
		result = (clockTick == alarmTick);
	}
	else {
		result = false;
	}

	clockTick++;

	// Make clock circular i.e. modulo
	if (clockTick >= Policy::CountSyncPeriodsToChooseMasterSyncXmits)
		wrap();

	return result;
}


void RandomAlarmingCircularClock::disarmForOneCycle() {
	isAlarmEnabled = false;
}




