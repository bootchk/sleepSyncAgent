#pragma once

#include "randomAlarmingClock.h"

/*
 * Thin wrapper (just an alias) on RandomAlarmingCircularClock
 */


class MasterTransmitSyncPolicy {
	static RandomAlarmingCircularClock clock;

public:
	static void reset() { clock.wrap(); }

	// Called every sync slot
	static bool shouldTransmitSync() { return clock.tickWithAlarm(); }

	static void advanceStage() { }  // Nothing.  Policy has only one stage.

	static void disarmForOneCycle() { clock.disarmForOneCycle(); }
};


