
#include <cassert>

#include "../../augment/random.h"

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


bool RandomAlarmingCircularClock::tick(){
	tickClock();
	return clockTick == alarmTick;
}


// private

void RandomAlarmingCircularClock::setAlarm() {
	alarmTick = randUnsignedInt16(0, TicksPerPeriod);
	assert(alarmTick <= TicksPerPeriod);
}

void RandomAlarmingCircularClock::tickClock() {
	clockTick++;
	// circular
	if (clockTick > TicksPerPeriod) reset();
}
