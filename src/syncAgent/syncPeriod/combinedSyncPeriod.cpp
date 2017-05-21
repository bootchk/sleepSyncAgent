
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

void fishOrMerge() {
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
}

}


/*
 * Each slot needs radio and radio requires HfCrystalClock.
 * Each slot is responsible for starting and stopping HfCrystalClock .
 */
void CombinedSyncPeriod::doSlotSequence() {

	// the caller, at schedule.rollPeriodForward has logged syncPoint

	// assert powerManager->isPowerForRadio()


	syncWorkSlot.perform();	// arbitrary to do sync slot first

	assert(network.isLowPower());	// until next slot

	// SyncSlot might have exhausted power
	if (powerManager->isPowerForRadio()) {
		fishOrMerge();
	}
	else {
		// DEBUG
		LogMessage::logExhaustedRadioPower();
	}

	assert(network.isLowPower());	// For remainder of sync period

	sleepRemainderOfSyncPeriod();

	// Sync period (slot sequence) completed
}
