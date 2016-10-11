
#include <cassert>

#include "../../../augment/random.h"
#include "policyParameters.h"

#include "masterXmitSyncPolicy.h"


// unsigned, counts upwards
ScheduleCount RandomAlarmingCircularClock::alarmTick;
ScheduleCount RandomAlarmingCircularClock::clockTick;


void RandomAlarmingCircularClock::reset() {
	// wrap clock circularly
	clockTick = 0;
	// set alarm when wrap
	setAlarm();
}

// FUTURE rename tickWithAlarm
// Returns true if alarm goes off
bool RandomAlarmingCircularClock::tick(){
	clockTick++;
	bool result = (clockTick == alarmTick);
	// Make clock circular
	if (clockTick > Policy::CountSyncPeriodsToChooseMasterSyncXmits) reset();
	return result;
	// assert alarm goes off when old clockTick was [0, CountSyncPeriodsToChooseMasterSyncXmits -1]
}


// private

void RandomAlarmingCircularClock::setAlarm() {
	alarmTick = randUnsignedInt16(0, Policy::CountSyncPeriodsToChooseMasterSyncXmits);
}


