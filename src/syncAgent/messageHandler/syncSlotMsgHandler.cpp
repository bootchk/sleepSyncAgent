
#include <cassert>

#include "../globals.h"		// clique, etc.
#include "messageHandler.h"
#include "../policy/workManager.h"
#include "../modules/syncBehaviour.h"
#include "../logging/logger.h"
#include "../syncAgent.h"
#include "../control/controller.h"
#include "../slots/fishing/fishingManager.h"



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

HandlingResult handleSyncAspectOfSyncCarryingMsg(SyncMessage* msg){

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


/*
 * Callbacks registered with FishingManager
 */
void endDeepFishingWithNoAction() {
	/*
	 * Self stays in current clique.
	 */
	// FUTURE remember so we aren't continually fished to another.
}

void endDeepFishingWithRecoverMaster() {
	/*
	 * Master left.
	 * Recover master if we haven't already heard another assuming mastership.
	 */
	// TODO poor strategy is self to assume

}

/*
 * Some other superior clique member fished us.
 * That other is sending this.
 * Try to DeepFish their master.
 * Stay in current clique until that succeeds.
 */
HandlingResult handleEnticingInferiorMessage(SyncMessage* msg){
	/*
	 * Conversion from DeltaSync to DeltaTime loses some rigor
	 * Type DeltaSync in message is a class instance.
	 * DeltaTime is dumber.
	 */
	FishingManager::switchToDeepFishing(msg->deltaToNextSyncPoint.get(), endDeepFishingWithNoAction);
	return HandlingResult::KeepListening;
}

/*
 * My master fished a superior clique and left.
 * My former master is sending me this.
 * Try to DeepFish the same superior master.
 * My current clique probably has no master until that succeeds.
 * If not succeed, try recover former master.
 */
HandlingResult handleMasterMergedAwayMessage(SyncMessage* msg){
	FishingManager::switchToDeepFishing(msg->deltaToNextSyncPoint.get(), endDeepFishingWithRecoverMaster);
	return HandlingResult::KeepListening;
}
/*
 * A slave in my clique fished a superior clique and left.
 * That slave is sending this.
 */
HandlingResult handleSlaveMergedAwayMessage(SyncMessage* msg){
	FishingManager::switchToDeepFishing(msg->deltaToNextSyncPoint.get(), endDeepFishingWithNoAction);
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
	/*
	 * Former design: MergeSync carried sync and receiver immediately merged,
	 * even if Master of other clique was not in range.
	 * That worked, and well, but only if all units could hear each other.
	 *
	 * case MessageType::MergeSync:
	 *	handlingResult = handleMergeSyncMessage(msg);
	 */
	case MessageType::EnticingInferior:
		handlingResult = handleEnticingInferiorMessage(msg);
		SyncAgent::countMergeSyncHeard++;
		break;
	case MessageType::MasterMergedAway:
		handlingResult = handleMasterMergedAwayMessage(msg);
		SyncAgent::countMergeSyncHeard++;
		break;
	case MessageType::SlaveMergedAway:
		handlingResult = handleSlaveMergedAwayMessage(msg);
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
	return handleSyncAspectOfSyncCarryingMsg(msg);
}

/*
OLD
HandlingResult SyncSlotMessageHandler::handleMergeSyncMessage(SyncMessage* msg){
	return handleSyncAspectOfSyncCarryingMsg(msg);
}
*/


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

	return handleSyncAspectOfSyncCarryingMsg(msg);
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



