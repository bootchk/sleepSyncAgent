
#include <cassert>

#include "syncWorkSlot.h"

#include "../../globals.h"	// clique

#include "../../modules/syncPowerManager.h"
#include "../../sleepers/scheduleSleeper.h"

#include "../../logging/logger.h"

#include "../../clique/clique.h"
#include "../../syncAgentImp/state/phase.h"



/*
 * SyncSlot is first.
 * Assert is enough power for this slot.
 */
void SyncWorkSlot::tryPerform() {

	/*
	 * Start network before deciding what kind of sync slot to perform.
	 */
	// Sleeps until ensemble ready. Deadtime in slot.
	// TIMING: > 360uSec, as much as 1.2mSec
	// Current:
	//LongTime startTime = clique.schedule.nowTime();
	Ensemble::startup();
	//LongTime endTime = clique.schedule.nowTime();

	// Starting network might have exhausted power.  Unlikely
	if (SyncPowerManager::isPowerForSyncSlot()) {
		Phase::set(PhaseEnum::SyncChooseKind);
		perform();
	}
	else {
		Phase::set(PhaseEnum::SyncSkipSlot);
		Logger::logNoPowerToStartSyncSlot();

		// Busted SyncSlot, no listen, no send sync
		ScheduleSleeper::sleepSyncSlotRemainder();

		// Continuation will be sleep to FishSlot

		// Maybe HFXO will start faster on second iterations?
	}
}



/*
 * Transmit any sync in middle of slot.
 * Some literature refers to "guards" around the sync.
 * Syncing in middle has higher probability of being heard by drifted/skewed others.
 *
 * !!! The offset must be half the slot length, back to start of SyncPeriod
 */

void SyncWorkSlot::perform() {
	// logInt(clique.schedule.deltaPastSyncPointToNow()); log("<delta SP to start slot.\n");

	// assert network is started

	dispatchSyncSlotKind();

	/*
	 * This may be late, when message receive thread delays this.
	 * Also, there could be a race to deliver message with this event.
	 * FUTURE check for those cases.
	 * Scheduling of subsequent events does not depend on timely this event.
	 */

	// Radio might be in use.  In use: we timeout'd while receiving
	Ensemble::stopReceiving();

	// FUTURE we could do this elsewhere, e.g. start of sync slot
	if (!clique.isSelfMaster())
		clique.checkMasterDroppedOut();

	Ensemble::shutdown();

	assert(Ensemble::isLowPower());
}

