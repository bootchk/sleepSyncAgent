
// Part of SSTask class implementation, specific to <fishing>
#include "task.h"

#include "../schedule/radioPrelude.h"
#include "../schedule/syncSchedule.h"

#include "../slots/fish/fishSlot.h"

#include "../syncAgentImp/state/role.h"
#include "../policy/fishWaitingPolicy.h"
#include "../modules/syncPowerManager.h"

// in old design?
#include "../slots/fishing/fishingManager.h"
#include "../slots/fishing/fishSchedule.h"

#include <cassert>
#include "../slots/merge/mergePolicy.h"



/*
 * Not scheduled, called at endSyncSlot
 */

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
			FishSchedule::setStartAndEndTimes();	// !!! calculate times for scheduling
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






/*
 * Scheduled tasks
 */

void SSTask::fishSlotStart() {
	/*
	 * Trolling or Deep
	 * Scheduling fishSlotEnd is polymorphic on mode.
	 */
	assert(RadioPrelude::isDone());
	SyncSchedule::fishSlotEnd();
	FishSlot::beginListen();
	// Two tasks active: radio and timer
}


void SSTask::fishSlotEnd() {
	// Trolling or Deep

	// Enhance: received message might already radio off and prelude off?
	// RadioPrelude::undo() ?

	FishSlot::endListen();	// Receiver::stop();
	assert(RadioPrelude::isDone());	// because sync slot might abut

	// Advances to next trolling slot or ends deep
	FishingManager::checkFishingDone();

	SyncSchedule::syncSlotAfterFishSlot();
}
