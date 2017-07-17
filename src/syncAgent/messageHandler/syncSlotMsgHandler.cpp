
#include <cassert>

#include "../globals.h"		// syncBehaviour, clique, etc.
#include "messageHandler.h"



/*
 * Message handling for SyncSlot.
 *
 * Sync handling is agnostic of role.isMaster or role.isSlave
 *
 * MasterSync and MergeSync handled the same.
 *
 * FUTURE discard multiple sync messages if they are queued
 */

HandlingResult SyncSlotMessageHandler::handle(SyncMessage* msg){
	HandlingResult handlingResult;

	switch(msg->type) {
	case MessageType::MasterSync:
		log(Logger::RXMasterSync);
		handlingResult = handleMasterSyncMessage(msg);
		break;
	case MessageType::MergeSync:
		log(Logger::RXMergeSync);
		handlingResult = handleMergeSyncMessage(msg);
		SyncAgent::countMergeSyncHeard++;
		break;
	case MessageType::AbandonMastership:
		log(Logger::RXAbandonMastership);
		handlingResult = handleAbandonMastershipMessage(msg);
		break;
	case MessageType::WorkSync:
		log(Logger::RXWorkSync);
		handlingResult = handleWorkSyncMessage(msg);
		break;
	default:
		/*
		 * Covers case where coder cast a semantically bad value into the enum class MessageType.
		 */
		assert(false);
	}

	return handlingResult;
}


HandlingResult SyncSlotMessageHandler::handleMasterSyncMessage(SyncMessage* msg){
	/*
	 * Discard result and keep listening since:
	 * - two masters may be competing
	 */
	(void) syncBehaviour.filterSyncMsg(msg);
	return HandlingResult::KeepListening;
}


HandlingResult SyncSlotMessageHandler::handleMergeSyncMessage(SyncMessage* msg){
	/*
	 * Discard result and keep listening since:
	 * - two other cliques may be competing to merge me
	 */
	(void) syncBehaviour.filterSyncMsg(msg);
	return HandlingResult::KeepListening;
}


HandlingResult SyncSlotMessageHandler::handleAbandonMastershipMessage(SyncMessage* msg){
	/*
	 * My clique is still in sync, but master is dropout.
	 *
	 * Naive design: all units that hear master abandon assume mastership.
	 * FUTURE: keep historyOfMasters, and better slaves assume mastership.
	 */
	(void) msg;  // FUTURE use msg to record history

	clique.setSelfMastership();
	assert(clique.isSelfMaster());
	return HandlingResult::KeepListening;
}


HandlingResult SyncSlotMessageHandler::handleWorkSyncMessage(SyncMessage* msg){
	/*
	 * Handle work aspect of message.
	 * Doesn't matter which clique it came from, relay work.
	 */
	workManager.hearWork();	// keep work state for slot
	syncAgent.relayHeardWorkToApp(msg->work);

	/*
	 *  Handle sync aspect of message.
	 *  Don't care if it is invalid from inferior clique.
	 */
	(void) syncBehaviour.filterSyncMsg(msg);

	/*
	 * Ignore sync-keeping result above and keep listening:
	 * - other masters may be competing
	 */
	// FUTURE if we hear many WorkSync, may relay all of them to app
	// FUTURE if our app is scheduling work and we already heard one
	// and work is generic (not carrying any info) we should not xmit WorkSync

	return HandlingResult::KeepListening;
}
