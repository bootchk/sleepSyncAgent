#pragma once

// This is not from platform, is local.
#include "../types.h" // ScheduleCount

/*
 * Specialized RNG Random Number Generator
 *
 * Generates value true periodically, but randomly.
 *
 * Circular clock having TicksPerPeriod wrap around.
 * Clock ticks when tickWithAlarm() is called.
 * tickWithAlarm() returns true once every TicksPerPeriod calls.
 * Thus duration between returns of true is at most 2*TicksPerPeriod.
 *
 * Metaphor: 24 hour clock with one hour hand
 * and one alarm hand that randomly changes by itself every midnight.
 * Unlike the usual clock, it is not self-powered: it ticks when you call tickWithAlarm().
 * Like usual clock, you can leave alarm set, but disable it.
 * Unlike usual clock, if you disable alarm today, it enables itself for the next day.
 */
class RandomAlarmingCircularClock {
public:
	/*
	 * Advance counting hand to midnight(0) and set alarm to random hour.
	 *
	 * I.E. initialize.  Usually called rarely.
	 */
	static void wrap();

	/*
	 * Advance counting hand one hour and return true if it coincides with alarm hand.
	 *
	 * Must be called steadily.
	 */
	static bool tickWithAlarm();

	/*
	 * Disable alarm in current period.
	 * Alarm will reenable itself in next period.
	 */
	static void disarmForOneCycle();
};
