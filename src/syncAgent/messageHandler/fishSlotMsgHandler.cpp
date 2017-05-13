
#include <cassert>

#include "messageHandler.h"
#include "../globals.h"


namespace {

void doFishedSyncMsg(SyncMessage* msg){
	/*
	 * Heard sync or worksync
	 *
	 * MasterSync may be better or worse.
	 * toMergerRole() handles both cases,
	 * but always toMerger(), either merging my clique or other clique.
	 */
	syncAgent.toMergerRole(msg);
	assert(role.isMerger());
	/*
	 * assert (schedule changed AND self is merging my former clique)
	 * OR (schedule unchanged AND self is merging other clique)
	 */
	// assert my schedule might have changed
}

} // namespace




/*
 * Intended catch: MasterSync from another clique's Master in its sync slot.
 */
HandlingResult FishSlotMessageHandler::handleMasterSyncMessage(SyncMessage* msg){
	log(LogMessage::FishedMasterSync);
	doFishedSyncMsg(msg);
	// Stop listening: self can't handle more than one, or slot is busy with another merge
	return StopListeningHeardMasterSync;
}

/*
 * Intended catch: another clique's Master or Slave sending WorkSync in its sync slot.
 *
 * Implementation for combined Work/Sync slot.
 * Work carries sync, identifies master of clique and time of slot.
 *
 * Alternatively, when separate Work slot,
 * can calculate the other clique's sync slot from the Work msg.
 */
HandlingResult FishSlotMessageHandler::handleWorkSyncMessage(SyncMessage* msg){
	log(LogMessage::FishedWorkSync);
	doFishedSyncMsg(msg);
	// Stop listening: self can't handle more than one, or slot is busy with another merge
	return StopListeningHeardWorkSync;
}


/*
 * Unintended catch: Other (master or slave)
 * is already xmitting into this time thinking it is SyncSlot of some third clique.
 * Ignore except to stop fishing this slot.
 */
HandlingResult FishSlotMessageHandler::handleMergeSyncMessage(SyncMessage* msg){
	log(LogMessage::FishedMergeSync);
	(void) msg;
	return StopListeningHeardMergeSync;
}


/*
 * AbandonMastership
 *
 * Unintended catch: Another clique's master is abandoning (exhausted power)
 * For now ignore but keep listening.
 * Should catch clique again later, after another member assumes mastership.
 */
HandlingResult FishSlotMessageHandler::handleAbandonMastershipMessage(SyncMessage* msg){
	log(LogMessage::FishedAbandonMastershipSync);
	(void) msg;
	return KeepListening;
}



/*
 * Work
 *
 * Alternative designs:
 * Unintended catch: Another clique's work slot.
 * - Separate work slot: Ignore. Should catch clique again later, when we fish earlier, at it's syncSlot.
 * - Separate work slot:  Since work slot is in fixed relation to syncSlot, calculate syncSlot of catch, and merge it.
 * - Combined Work/Sync slot:
 *
 * In all alternatives: if work can be done when out of sync, do work.
 * onWorkMsgCallback(msg);	// Relay to app
 *
 */
