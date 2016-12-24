
#include <cassert>

#include "../globals.h"
#include "syncPeriod.h"

#include "../slots/syncWorkSlot.h"
#include "../slots/fishSlot.h"
#include "../slots/mergeSlot.h"

namespace {
SyncWorkSlot syncWorkSlot;
FishSlot fishSlot;
MergeSlot mergeSlot;
}

void CombinedSyncPeriod::doSlotSequence() {

	// Schedule.rollPeriodForward logs syncPoint

	// first, arbitrary
	syncWorkSlot.perform();

	assert(!radio->isPowerOn());	// Low power until next slot

	// Variation: next event (if any) occurs within a large sleeping time (lots of 'slots')
	if (role.isMerger()) {
		// avoid collision
		if (mergeSlot.mergePolicy.shouldScheduleMerge())  {
			mergeSlot.perform();
			// We might have quit role Merger
		}
		// else continue and sleep until end of sync period
	}
	else {
		// Fish every period
		fishSlot.perform();
		// continue and sleep until end of sync period
	}
	assert(!radio->isPowerOn());	// Low power for remainder of this sync period

	syncSleeper.sleepUntilTimeout(clique.schedule.deltaNowToNextSyncPoint);
	// Sync period completed
}
