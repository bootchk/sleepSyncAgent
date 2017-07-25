#pragma once

#include "../../../augment/randomAlarmingClock.h"
#include "../policyParameters.h"

/*
 * Thin wrapper (just an alias) on RandomAlarmingCircularClock
 */


class MasterTransmitSyncPolicy {
	static RandomAlarmingCircularClock clock;

public:

	static void init() {
		clock.init(Policy::CountSyncPeriodsToChooseMasterSyncXmits);
	}

	static void reset() {
		init();	// TODO do this once, earlier
		clock.wrap();
	}

	// Called every sync slot
	static bool shouldTransmitSync() { return clock.tickWithAlarm(); }

	static void advanceStage() { }  // Nothing.  Policy has only one stage.

	static void disarmForOneCycle() { clock.disarmForOneCycle(); }
};


