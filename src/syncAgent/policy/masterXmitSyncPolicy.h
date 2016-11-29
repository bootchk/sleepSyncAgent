#pragma once

#include "randomAlarmingClock.h"

/*
 * Thin wrapper (just an alias) on RandomAlarmingCircularClock
 */


class MasterXmitSyncPolicy {
	static RandomAlarmingCircularClock clock;

public:
	static void reset() { clock.wrap(); }

	// Called every sync slot
	static bool shouldXmitSync() { return clock.tickWithAlarm(); }

	static void advanceStage() { }  // Nothing.  Policy has only one stage.

	static void disarmForOneCycle() { clock.disarmForOneCycle(); }
};


