
#include <cassert>

#include "../globals.h"
#include "syncSlotMsgHandler.h"
#include "../logMessage.h"


HandlingResult SyncSlotMessageHandler::dispatchMsgReceived(SyncMessage* msg) {
	HandlingResult foundDesiredMessage = false;

	switch(msg->type) {
	case MasterSync:
		log(LogMessage::RXMasterSync);
		foundDesiredMessage = SyncSlotMessageHandler::doMasterSyncMsg(msg);
		break;
	case MergeSync:
		log(LogMessage::RXMergeSync);
		foundDesiredMessage = SyncSlotMessageHandler::doMergeSyncMsg(msg);
		break;
	case AbandonMastership:
		log(LogMessage::RXAbandonMastership);
		foundDesiredMessage = SyncSlotMessageHandler::doAbandonMastershipMsg(msg);
		break;
	case WorkSync:
		log(LogMessage::RXWorkSync);
		foundDesiredMessage = SyncSlotMessageHandler::doWorkMsg(msg);
		break;
	default:
		log(LogMessage::RXUnknown);
	}

	return foundDesiredMessage;
}


/*
 * Message handlers.
 *
 * Sync handling is agnostic of role.isMaster or role.isSlave
 *
 * MasterSync and MergeSync handled the same.
 *
 * FUTURE discard multiple sync messages if they are queued
 */


HandlingResult SyncSlotMessageHandler::doMasterSyncMsg(SyncMessage* msg){
	(void) syncBehaviour.doSyncMsg(msg);
	return false;	// keep looking
}


HandlingResult SyncSlotMessageHandler::doMergeSyncMsg(SyncMessage* msg){
	(void) syncBehaviour.doSyncMsg(msg);
	return false;
}


HandlingResult SyncSlotMessageHandler::doAbandonMastershipMsg(SyncMessage* msg){
	/*
	 * My clique is still in sync, but master is dropout.
	 *
	 * Naive design: all units that hear master abandon assume mastership.
	 * FUTURE: keep historyOfMasters, and better slaves assume mastership.
	 */
	(void) msg;  // FUTURE use msg to record history

	clique.setSelfMastership();
	assert(clique.isSelfMaster());
	return false;	// keep listening
}


HandlingResult SyncSlotMessageHandler::doWorkMsg(SyncMessage* msg){
	/*
	 * Handle work aspect of message.
	 * Doesn't matter which clique it came from, relay work.
	 */
	syncAgent.relayWorkToApp(msg->getWorkPayload());

	/*
	 *  Handle sync aspect of message.
	 */
	syncBehaviour.doSyncMsg(msg);

	return false;	// keep looking
}
