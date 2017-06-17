
#include "syncState.h"
#include "../globals.h"
// #include "../modules/syncSender.h"


namespace {

bool isSyncing = false;


/*
 * Things to do on state changes.
 */

/*
 * Not enough power to use radio for full sync slot.
 * Ask an other unit in my clique to assume mastership.
 * Might not be heard, in which case other units should detect DropOut.
 *
 * Takes minimal power, for a short transmittal.
 */
void doDyingBreath() {
	// TODO this should be in the middle of the SyncSlot
	phase = Phase::AbandonMastership;
	syncSender.sendAbandonMastership();
}


/*
 * We were not keeping sync, but about to start tying again (using radio.)
 */
void resumeSyncing() {
	// FUTURE onSyncingResumedCallback();	// Tell app
}

/*
 * Not enough power for self to continue syncing.
 * Other units might still have power and assume mastership of my clique
 */
void pauseSyncing() {

	// FUTURE if clique is probably not empty
	if (clique.isSelfMaster()) {
		doDyingBreath();
		// TODO am I still master?
	}
	// else I am a slave, just drop out of clique, others may have enough power

	// FUTURE onSyncingPausedCallback();	// Tell app
}



}



void SyncState::setActive() {
	if ( ! isSyncing ){
		// Change state
		isSyncing = true;
		// LogMessage::logStartSync();	// Log that we tried to listen/send sync again.
		resumeSyncing();
	}
	// else we are still syncing, might be master or slave
}

void SyncState::setPaused() {
	if ( isSyncing ){
		// Change state
		isSyncing = false;
		// LogMessage::logPauseSync();	// Log that we fell out
		pauseSyncing();
	}
}

