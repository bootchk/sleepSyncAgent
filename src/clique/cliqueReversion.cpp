/*
 * Part of implementation of Clique:
 * aspects:
 * - revert clique to former clique
 */

#include "clique.h"

#include "../policy/syncPolicy/adaptiveXmitSyncPolicy.h"
#include "../cliqueHistory/cliqueHistory.h"
#include "../slots/fishing/fishingManager.h"

#include "../logging/logger.h"



/*
 * Other Slaves might do this and engender contention (many Masters.)
 */
void Clique::grabMastership() {
	// !!! changes role: self will start xmitting sync
	setSelfMastership();

	// Reset so we don't xmit sync soon
	resetTransmitSyncPolicy();

	/*
	 * !!! Schedule is NOT changed.
	 * We may be able to recover the master we lost by fishing nearby.
	 * Also, we will sooner hear other slaves also grabbing mastership
	 */

	/*
	 * Reset fishing to troll slot near syncSlot.
	 * This might help recover a Master who didn't permanently drop out.
	 */
	FishingManager::restartTrollingMode();
	// WAS SyncRecoveryFishPolicy::reset();

	// DropoutMonitor::reset() is not necessary since only slaves use it
	// It will get reset when we become a slave.
}


void Clique::revertToFormerMaster() {
	// assert currentCliqueRecord is the former one
	updateMastership(CliqueHistory::currentCliqueRecordMasterID());

	// syncSlot at time of former master
	schedule.adjustByCliqueHistoryOffset(CliqueHistory::offsetToCurrentClique());

	// Reset fishing policy to fish slot near syncSlot.
	// Former master may have drifted.
	FishingManager::restartTrollingMode();
	// WAS SyncRecoveryFishPolicy::reset();
}


void Clique::revertByCliqueHistory(){
	CliqueHistory::setCurrentCliqueRecordToFormerClique();

	if (CliqueHistory::isCurrentCliqueRecordSelf()) {
		grabMastership();
		// Self is master
	}
	else {
		revertToFormerMaster();
	}
	// TODO resetTransmitSyncPolicy() ???
}



void Clique::onMasterDropout() {
	/*
	 * Self unit has not heard sync from any member for a long time.
	 *
	 * Cases:
	 * - Master busy or insufficient power temporarily
	 * - Self busy or insufficient power temporarily
	 * - mutual drift too much
	 * - we were told to merge to a Master that is out of our range
	 *
	 * Make choice of strategy/behavior
	 */
	Logger::logMasterDropout();

	revertByCliqueHistory();
}
