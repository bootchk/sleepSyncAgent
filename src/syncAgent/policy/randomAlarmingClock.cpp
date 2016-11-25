
#include <cassert>

#include "../../augment/random.h"
#include "policyParameters.h"

#include "randomAlarmingClock.h"


/*
 * !!! Not a general implementation.
 * Uses constants that should be passed in the constructor.
 * Uses specific types from the application (SleepSync).
 */

// unsigned, counts upwards
ScheduleCount RandomAlarmingCircularClock::alarmTick;
ScheduleCount RandomAlarmingCircularClock::clockTick;


void RandomAlarmingCircularClock::wrap() {
	clockTick = 0;
	setAlarm();
}


// Returns true if alarm goes off
bool RandomAlarmingCircularClock::tickWithAlarm(){
	// clockTick is unsigned => positive or zero
	assert(clockTick <= Policy::CountSyncPeriodsToChooseMasterSyncXmits-1);

	bool result = (clockTick == alarmTick);

	// Make clock circular
	clockTick++;
	if (clockTick >= Policy::CountSyncPeriodsToChooseMasterSyncXmits)
		wrap();

	return result;
	// assert alarm goes off when old clockTick was [0, CountSyncPeriodsToChooseMasterSyncXmits -1]
}


// private

void RandomAlarmingCircularClock::setAlarm() {
	alarmTick = randUnsignedInt16(0, Policy::CountSyncPeriodsToChooseMasterSyncXmits-1);
	// alarmTick in [0, CountSyncPeriodsToChooseMasterSyncXmits-1]
}


