
#include <cassert>

#include "random.h"

#include "randomAlarmingClock.h"


/*
 * !!! Not a general implementation.
 * Uses constants that should be passed in the constructor.
 * Uses specific types from the application (SleepSync).
 */




namespace {

bool isAlarmEnabled;

// unsigned, counts upwards
ClockCount alarmTick;
ClockCount clockTick;

ClockCount cycleLength;


void setAlarm() {
	// cast to eliminate conversion warning
	alarmTick = randUnsignedInt16(0, (ClockCount) (cycleLength - 1));
	// alarmTick in [0, CountSyncPeriodsToChooseMasterSyncXmits-1]
}

} // namespace




void RandomAlarmingCircularClock::init(ClockCount cycle) { cycleLength = cycle; }


void RandomAlarmingCircularClock::wrap() {
	clockTick = 0;
	setAlarm();
	isAlarmEnabled = true;
}


// Returns true if alarm goes off
bool RandomAlarmingCircularClock::tickWithAlarm(){
	// clockTick is unsigned => positive or zero
	assert(clockTick <= cycleLength-1);

	bool result;
	if (isAlarmEnabled) {
		result = (clockTick == alarmTick);
	}
	else {
		result = false;
	}

	clockTick++;

	// Make clock circular i.e. modulo
	if (clockTick >= cycleLength)
		wrap();

	return result;
}


void RandomAlarmingCircularClock::disarmForOneCycle() {
	isAlarmEnabled = false;
}




