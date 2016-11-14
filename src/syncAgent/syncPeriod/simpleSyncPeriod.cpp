
#include <cassert>
#include "../../platform/platform.h"
#include "../globals.h"
#include "syncPeriod.h"

#include "../slots/syncSlot.h"
#include "../slots/workSlot.h"
#include "../slots/fishSlot.h"
#include "../slots/mergeSlot.h"

namespace {
// 4 slot types, 3 active per sync period
SyncSlot syncSlot;
WorkSlot workSlot;
FishSlot fishSlot;
MergeSlot mergeSlot;
}

void SimpleSyncPeriod::doSlotSequence() {

	// log("Now time\n");
	// logLongLong(clique.schedule.nowTime());


	// syncSlot first, arbitrary
	syncSlot.perform();

	#ifdef FUTURE
	// Variation: work slot may be merging
	if (role.isWorkMerger()) {
		workSlot.performWorkMerger();
	}
	else {
		workSlot.performWork();
	}
#endif


#ifdef SYNC_AGENT_CONVEYS_WORK
	// TODO also ORDINAL OF FIRST FISHING SLOT
	workSlot.performWork();
#endif

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
