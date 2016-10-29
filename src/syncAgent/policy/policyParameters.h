
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

	// After role Slave fail to hear this count of MasterSyncs, revert to role Master
	static const ScheduleCount maxMissingSyncsPerDropout = 20;

	// Role Master xmits MasterSync once per this many SyncPeriods, in a random one of them.
	// The max span between MasterSyncs xmitted can be twice this
	// The max span between MasterSyncs heard can be much greater, because of contention
	static const ScheduleCount CountSyncPeriodsToChooseMasterSyncXmits = 3;
};
