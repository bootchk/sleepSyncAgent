
// Part of SSTask class implementation, specific to fishing
#include "task.h"

#include "../schedule/radioPrelude.h"
#include "../schedule/syncSchedule.h"

#include "../slots/fish/fishSlot.h"

#include "../syncAgentImp/state/role.h"
#include "../policy/fishWaitingPolicy.cpp"
#include "../modules/syncPowerManager.h"

// in old design?
#include "../slots/merging/mergePolicy.h"
#include "../slots/fishing/fishingManager.h"

#include <cassert>

/*
 * Try means: may lack power.
 * Variation: next event (if any) occurs within a large sleeping time (lots of 'slots').
 * We don't check power here because it may recover during sleep until time to perform.
 */
void SSTask::tryFishOrMerge() {
	switch(MergerFisherRole::role()) {
	case Role::Merger:
		// avoid collision
		if (MergePolicy::shouldScheduleMerge())  {
			SyncSchedule::merger();
			// We might have quit role Merger
		}
		else SyncSchedule::omitNonsyncSlot();
		break;
	case Role::Fisher:
		/*
		 * Fish every period.
		 *
		 * Check power early, not just before fishing.
		 */
		if (SyncPowerManager::isPowerForFishSlot()) {
			SyncSchedule::fishing();
		}
		else SyncSchedule::omitNonsyncSlot();
		break;
	case Role::NoFishing:
		/*
		 * May change to Role::Fishing
		 * Unimplemented: if we can pause fishing to disrupt order of fishing.
		 */
		FishWaitingPolicy::advance();
		SyncSchedule::omitNonsyncSlot();
	}
	/*
	 * A nonsync task is scheduled, and RadioPrelude might be done
	 * OR RadioPrelude not done and prelude to next sync slot is scheduled.
	 */
}



void SSTask::scheduleFishSlot() {
	if (RadioPrelude::isDone()) {
		SyncSchedule::fishSlotStart();
	}
	else {
		SyncSchedule::radioPreludeTaskWFish();
	}
}



void SSTask::fishSlotStart() {
	assert(RadioPrelude::isDone());
	SyncSchedule::fishSlotEnd();
	FishSlot::beginListen();
	// Two tasks active: radio and timer
}

void SSTask::fishSlotEnd() {

	// Possible change role
	FishingManager::checkFishingDone();

	SyncSchedule::syncSlotAfterFishSlot();
}

void SSTask::provisionStart() {
}


void SSTask::mergerStart() {

}
