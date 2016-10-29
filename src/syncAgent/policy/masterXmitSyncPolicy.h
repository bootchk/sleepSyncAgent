#pragma once

// TODO is this from platform??? #include "../../../platform/platform.h"	// types.h
#include "../types.h" // ScheduleCount

/*
 * Specialized RNG Random Number Generator
 *
 * Generates true periodically, but randomly.
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



/*
 * Thin wrapper (just an alias) on RandomAlarmingCircularClock
 */
class MasterXmitSyncPolicy {
	static RandomAlarmingCircularClock clock;
public:
	static void reset() { clock.wrap(); }

	// Called every sync slot
	static bool shouldXmitSync() { return clock.tickWithAlarm(); }
};


