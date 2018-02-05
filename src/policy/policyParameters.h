
#pragma once

#include "../types.h"

/*
 * Constants that define policy
 *
 * Not instantiated.
 * Included and referenced by several policy implementors.
 *
 * This file gathers parameters in one place, but each is only used in one place.
 */

//

class Policy {
public:
	// After role Merger xmits this count of MergeSync, revert to role Fisher
	static const int CountSyncsPerMerger = 6;

	/*
	 * After role Slave fails to hear this count of sync keeping msg, revert to role Master.
	 *
	 * Derive from xtal drift of say 20ppm and length of sync slot in ticks.
	 * Sync periods to drift out of sync is (1/ppm) / (ticks/period)
	 * Drifts of master and slaves could be in opposite directions, so factor of two.
	 *
	 * Typically, slave will drift half a sync slot in 50 sync periods.
	 */
	//static const ScheduleCount maxMissingSyncsPerDropout = 40;
	static const EventCount maxMissingSyncsPerDropout = 80;

	/*
	 * Role Master xmits MasterSync once per this many SyncPeriods, in a random one of them.
	 * The max span between MasterSyncs xmitted can be twice this
	 * The max span between MasterSyncs heard can be much greater, because of contention
	 *
	 * !!! Note some policies are adaptive, and will use a different parameter sometimes.
	 */
	// Original concept: every third period
	static const EventCount CountSyncPeriodsToChooseMasterSyncXmits = 3;

	/*
	 * !!! Only for testing DutyCycle and adaptiveSyncing: every period
	 * Gives too much contention.
	 */
	// static const ScheduleCount CountSyncPeriodsToChooseMasterSyncXmits = 1;
};
