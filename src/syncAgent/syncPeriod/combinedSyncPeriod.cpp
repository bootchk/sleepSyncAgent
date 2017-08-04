
#include <cassert>

#include "../globals.h"	// clique
#include "syncPeriod.h"

#include "../state/phase.h"
#include "../state/role.h"

#include "../slots/syncWorkSlot.h"
#include "../slots/fishSlot.h"
#include "../slots/mergeSlot.h"

#include "../modules/clique.h"

#include "../policy/fishWaitingPolicy.h"


namespace {
SyncWorkSlot syncWorkSlot;
FishSlot fishSlot;
MergeSlot mergeSlot;


void sleepRemainderOfSyncPeriod() {
	SyncSleeper::sleepUntilTimeout(clique.schedule.deltaNowToNextSyncPoint);
}

void tryFishOrMerge() {
	/*
	 * Try means: may lack power.
	 * Variation: next event (if any) occurs within a large sleeping time (lots of 'slots').
	 * We don't check power here because it may recover during sleep until time to perform.
	 */
	switch(MergerFisherRole::role) {
	case Role::Merger:
		// avoid collision
		if (mergeSlot.mergePolicy.shouldScheduleMerge())  {
			mergeSlot.tryPerform();
			// We might have quit role Merger
		}
		// else continue and sleep until end of sync period
		break;
	case Role::Fisher:
		// Fish every period
		fishSlot.tryPerform();
		// continue and sleep until end of sync period
		break;
	case Role::Waiting:
		/*
		 * May change to Role::Fishing
		 */
		FishWaitingPolicy::advance();
		/*
		 * Otherwise, do no fishing or merging.
		 */
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
