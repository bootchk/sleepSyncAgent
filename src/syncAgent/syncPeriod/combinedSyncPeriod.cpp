
#include <cassert>

#include "../globals.h"
#include "syncPeriod.h"

#include "../slots/syncWorkSlot.h"
#include "../slots/fishSlot.h"
#include "../slots/mergeSlot.h"

#include "../logMessage.h"

namespace {
SyncWorkSlot syncWorkSlot;
FishSlot fishSlot;
MergeSlot mergeSlot;


void sleepRemainderOfSyncPeriod() {
	syncSleeper.sleepUntilTimeout(clique.schedule.deltaNowToNextSyncPoint);
}

void tryFishOrMerge() {
	/*
	 * Try means: may lack power.
	 * Variation: next event (if any) occurs within a large sleeping time (lots of 'slots').
	 * We don't check power here because it may recover during sleep until time to perform.
	 */
	if (role.isMerger()) {
		// avoid collision
		if (mergeSlot.mergePolicy.shouldScheduleMerge())  {
			mergeSlot.tryPerform();
			// We might have quit role Merger
		}
		// else continue and sleep until end of sync period
	}
	else {
		// Fish every period
		fishSlot.tryPerform();
		// continue and sleep until end of sync period
	}
}

}


/*
 * Each slot needs radio and radio requires HfCrystalClock.
 * Each slot is responsible for starting and stopping HfCrystalClock .
 */
void CombinedSyncPeriod::doSlotSequence() {

	// the caller, at schedule.rollPeriodForward has logged syncPoint

	// assert powerManager->isPowerForSync() which is higher than isPowerForRadio

	Phase::set(PhaseEnum::StartSlotSequence);
	syncWorkSlot.tryPerform();	// arbitrary to do sync slot first

	assert(Ensemble::isLowPower());	// until next slot

	/*
	 * SyncSlot might have exhausted power.
	 * As we proceed, we check power again just before we need the power.
	 */
    tryFishOrMerge();

	assert(Ensemble::isLowPower());	// For remainder of sync period

	Phase::set(PhaseEnum::SleepRemainder);
	sleepRemainderOfSyncPeriod();

	// Sync period (slot sequence) completed
}
