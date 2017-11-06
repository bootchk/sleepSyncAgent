
#include <cassert>

#include "../globals.h"		// clique, etc.
#include "messageHandler.h"
#include "../policy/workManager.h"
#include "../modules/syncBehaviour.h"
#include "../logging/logger.h"
#include "../syncAgent.h"
#include "../control/controller.h"



/*
 * Message handling for SyncSlot.
 *
 * Sync handling is agnostic of isMaster or isSlave
 *
 * MasterSync and MergeSync handled the same.
 *
 * FUTURE discard multiple sync messages if they are queued
 */


namespace {

HandlingResult handleSyncAspectOfSyncMsg(SyncMessage* msg){
	/*
	 * Filter: only respond to superior or self's clique.
	 */
	bool heardSync = SyncBehaviour::filterSyncMsg(msg);

	/*
	 * This implements strategy for multiple superior syncs heard in the same SyncSlot.
	 *
	 * Simplest strategy:
	 * don't listen for more sync and don't send our own sync.
	 *
	 * One strategy is to keep listening since:
	 * - two masters may be competing
	 * But there are bugs here: get assertion invalid OTA offset if we try to send our own MasterSync
	 * after adjusting schedule by a Sync.
	 */
	if (heardSync)
		return HandlingResult::StopListeningHeardMasterSync;	// TODO just some flavor of sync
	else
		return HandlingResult::KeepListening;
}

}  // namespace




HandlingResult SyncSlotMessageHandler::handle(SyncMessage* msg){
	HandlingResult handlingResult;

	Logger::log("Sync ");
	Logger::logReceivedMsg(msg);

	switch(msg->type) {
	case MessageType::MasterSync:
		handlingResult = handleMasterSyncMessage(msg);
		break;
	case MessageType::MergeSync:
		handlingResult = handleMergeSyncMessage(msg);
		SyncAgent::countMergeSyncHeard++;
		break;
	case MessageType::WorkSync:
		handlingResult = handleWorkSyncMessage(msg);
		break;
	case MessageType::AbandonMastership:
		handlingResult = handleAbandonMastershipMessage(msg);
		break;
	case MessageType::Info:
	case MessageType::WorkSetProximity:
	case MessageType::WorkScatterTime:
		handlingResult = handleInfoMessage(msg);
		break;
	case MessageType::ControlSetXmitPower:
	case MessageType::ControlScatterClique:
		handlingResult = handleControlMessage(msg);
		break;
	}

	return handlingResult;
}


HandlingResult SyncSlotMessageHandler::handleMasterSyncMessage(SyncMessage* msg){
	return handleSyncAspectOfSyncMsg(msg);
}

HandlingResult SyncSlotMessageHandler::handleMergeSyncMessage(SyncMessage* msg){
	return handleSyncAspectOfSyncMsg(msg);
}

HandlingResult SyncSlotMessageHandler::handleWorkSyncMessage(SyncMessage* msg){
	/*
	 * Handle work aspect of message.
	 * Doesn't matter which clique it came from, relay work.
	 */
	WorkManager::hearWork();	// keep work state for slot
	SyncAgent::relayHeardWorkToApp(msg->work);

	/*
	 * TODO revisit and consolidate these comments
	 * Ignore sync-keeping result above and keep listening:
	 * - other masters may be competing
	 */
	// FUTURE if we hear many WorkSync, may relay all of them to app
	// FUTURE if our app is scheduling work and we already heard one
	// and work is generic (not carrying any info) we should not xmit WorkSync

	return handleSyncAspectOfSyncMsg(msg);
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




HandlingResult SyncSlotMessageHandler::handleInfoMessage(SyncMessage* msg){
	Logger::logReceivedInfo(msg->work);
	return HandlingResult::KeepListening;
}


HandlingResult SyncSlotMessageHandler::handleControlMessage(SyncMessage* msg){
	Controller::setXmitPower(msg->work);
	return HandlingResult::KeepListening;
}



