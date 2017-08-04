
#include "syncState.h"

#include "../logging/logger.h"


namespace {

bool isSyncing = false;


/*
 * Things to do on state changes.
 */


/*
 * We were not keeping sync, but about to start tying again (using radio.)
 */
void resumeSyncing() {
	// FUTURE onSyncingResumedCallback();	// Tell app
}

/*
 * Not enough power for self to do a normal SyncPeriod (SyncSlot and Fish/MergeSlot)
 * TODO And xmit?  And fish?
 * TODO if can listen but not fish, should do that.
 * TODO if c
 *
 * Trigger a state machine that might eventually abandon mastership.
 * Other units might still have power and assume mastership of my clique
 */
void pauseSyncing() {
	// TODO count the consecutive times this has happened.

	// FUTURE onSyncingPausedCallback();	// Tell app
}



} // namespace



void SyncState::setActive() {
	if ( ! isSyncing ){
		// Change state
		isSyncing = true;
		//LogMessage::logStartSync();	// Log that we tried to listen/send sync again.
		resumeSyncing();
	}
	// else we are still syncing, might be master or slave
}

void SyncState::setPaused() {
	if ( isSyncing ){
		// Change state
		isSyncing = false;
		pauseSyncing();
	}
}


bool SyncState::shouldAbandonMastership() {
	// TODO abandon
	return false;
	/*
	 if (clique.isSelfMaster()) {
	     result = true;
			doDyingBreath();
			// TODO am I still master?
		}
		// else I am a slave, just drop out of clique, others may have enough power
		*/
	// When it is more than a few, and there is enough power to xmit,

		// FUTURE if clique is probably not empty
}

