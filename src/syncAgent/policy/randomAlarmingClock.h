#pragma once

// This is not from platform, is local.
#include "../types.h" // ScheduleCount

/*
 * Specialized RNG Random Number Generator
 *
 * Generates value true periodically, but randomly.
 *
 * Circular clock having TicksPerPeriod wrap around.
 * Clock ticks when shouldXmitSync() is called.
 * Returns true once every TicksPerPeriod calls to shouldXmitSync().
 * Thus duration between returns of true is at most 2*TicksPerPeriod.
 */
class RandomAlarmingCircularClock {
public:
	static void wrap();
	static bool tickWithAlarm();

private:
	static ScheduleCount alarmTick;
	static ScheduleCount clockTick;

	static void setAlarm();
};
