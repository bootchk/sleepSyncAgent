#pragma once

#include "../types.h"


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
	static void reset();
	static bool tick();

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
	static void reset() { clock.reset(); }

	// Called every sync slot
	static bool shouldXmitSync() { return clock.tick(); }
};


