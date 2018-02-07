
#include <cassert>

#include "messageHandler.h"

#include "../logging/logger.h"
#include "../syncAgent/state/role.h"
#include "../syncAgent/syncAgent.h"
#include "../slots/fishing/fishingManager.h"



HandlingResult FishSlotMessageHandler::doFishedSyncMsg(SyncMessage* msg){
	/*
	 * Heard sync or worksync
	 *
	 * MasterSync may be better or worse.
	 * toMergerRole() handles both cases,
	 * but always toMerger(), either merging my clique or other clique.
	 */
	HandlingResult result;

	switch(FishingManager::mode()){
	case FishingMode::Trolling:
		SyncAgent::toMergerFromFisher(msg);

		// Stop listening: self can't handle more than one, or slot is busy with another merge
		result = HandlingResult::StopListeningHeardMasterSync;
		/*
		 * assert (schedule changed AND self is merging my former clique)
		 * OR (schedule unchanged AND self is merging other clique)
		 */
		break;
	case FishingMode::DeepFishing:
		result = FishSlotMessageHandler::handleCatchFromDeepFishing(msg);
		break;
	}

	// assert my schedule might have changed
	// assert (Role is Fishing and FishingMode is Trolling) or role is merger
	return result;
}




HandlingResult FishSlotMessageHandler::handle(SyncMessage* msg){
	HandlingResult handlingResult;

	Logger::logReceivedMsg(msg);

	switch(msg->type) {
	case MessageType::MasterSync:
		handlingResult = handleMasterSyncMessage(msg);
		break;
	case MessageType::WorkSync:
		handlingResult = handleWorkSyncMessage(msg);
		break;

	case MessageType::MasterMergedAway:
	case MessageType::SlaveMergedAway:
	case MessageType::EnticingInferior:
		handlingResult = handleMergeSyncMessage(msg);
		break;

	case MessageType::AbandonMastership:
		handlingResult = handleAbandonMastershipMessage(msg);
		break;



	case MessageType::Info:
	case MessageType::WorkSetProximity:
	case MessageType::WorkScatterTime:
		handlingResult = CommonMessageHandler::handleInfoMessage(msg);
		break;

	case MessageType::ControlNetGranularity:
		handlingResult = CommonMessageHandler::handleControlMessageSetGranularity(msg);
		break;

	case MessageType::ControlScatterClique:
		handlingResult = CommonMessageHandler::handleControlMessageScatter(msg);
		break;
	}

	return handlingResult;
}



/*
 * Intended catch: MasterSync from another clique's Master in its sync slot.
 */
HandlingResult FishSlotMessageHandler::handleMasterSyncMessage(SyncMessage* msg){
	return doFishedSyncMsg(msg);

	//return HandlingResult::StopListeningHeardMasterSync;
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
	doFishedSyncMsg(msg);
	// Stop listening: self can't handle more than one, or slot is busy with another merge
	return HandlingResult::StopListeningHeardWorkSync;
}


/*
 * Unintended catch: Other (master or slave)
 * is already xmitting into this time thinking it is SyncSlot of some third clique.
 * Ignore except to stop fishing this slot.
 */
HandlingResult FishSlotMessageHandler::handleMergeSyncMessage(SyncMessage* msg){
	Logger::log(Logger::FishedMergeSync);
	(void) msg;
	return HandlingResult::StopListeningHeardMergeSync;
}


/*
 * AbandonMastership
 *
 * Unintended catch: Another clique's master is abandoning (exhausted power)
 * For now ignore but keep listening.
 * Should catch clique again later, after another member assumes mastership.
 */
HandlingResult FishSlotMessageHandler::handleAbandonMastershipMessage(SyncMessage* msg){
	Logger::log(Logger::FishedAbandonMastershipSync);
	(void) msg;
	return HandlingResult::KeepListening;
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

