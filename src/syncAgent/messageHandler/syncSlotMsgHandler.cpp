
#include <cassert>

#include "../globals.h"		// clique, etc.
#include "messageHandler.h"
#include "../policy/workManager.h"
#include "../modules/syncBehaviour.h"
#include "../logging/logger.h"
#include "../syncAgent.h"
#include "../scheduleParameters.h"

#include "../slots/fishing/fishingManager.h"
#include "../slots/fishing/fishSlot.h"



/*
 * Message handling for SyncSlot.
 *
 * Sync handling is agnostic of isMaster or isSlave
 *
 * FUTURE discard multiple sync messages if they are queued
 */


namespace {

/*
 * Sync carrying messages
 */
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
 * Merge class of messages.
 *
 * Formerly, MergeSync messages adjusted sync (joined other clique) immediately.
 * Now, we DeepFish for other clique before joing it.
 */

/*
 * Just received msg, of class Merge.
 * It's offset is somewhat adjusted delta from now to sync point of another clique.
 *
 * The result we want is a delta from our sync point to sync point of another clique.
 * Result is used later (when we schedule fishing).

 * But now is sometime in sync slot.
 * So add backward delta from our sync point to now.
 * Which is equal to slot duration minus delta from now to end of SyncSlot
 */
DeltaTime getFishingDeltaFromMergeMsg(SyncMessage* msg) {
	LongTime timeToFish = clique.schedule.adjustedEventTimeFromMsg(msg);
	DeltaTime deltaToFish = TimeMath::clampedTimeDifferenceFromNow(timeToFish);

	// We haven't changed cliques, schedule is unchanged (or slightly changed) since we started SyncSlot
	DeltaTime result = deltaToFish + clique.schedule.deltaPastSyncPointToNow();

	// No assertions here, assertions later when we use it.
	return result;
}

/*
 * Some other superior clique member fished us.
 * That other is sending this.
 * Try to DeepFish their master.
 * Stay in current clique until that succeeds.
 */
HandlingResult handleEnticingInferiorMessage(SyncMessage* msg){
	FishingManager::switchToDeepFishing(getFishingDeltaFromMergeMsg(msg), FishSlot::endDeepFishingWithNoAction);
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
	FishingManager::switchToDeepFishing(getFishingDeltaFromMergeMsg(msg), FishSlot::endDeepFishingWithRecoverMaster);
	return HandlingResult::KeepListening;
}
/*
 * A slave in my clique fished a superior clique and left.
 * That slave is sending this.
 */
HandlingResult handleSlaveMergedAwayMessage(SyncMessage* msg){
	FishingManager::switchToDeepFishing(getFishingDeltaFromMergeMsg(msg), FishSlot::endDeepFishingWithNoAction);
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
		handlingResult = CommonMessageHandler::handleInfoMessage(msg);
		break;
	case MessageType::ControlSetXmitPower:
	case MessageType::ControlScatterClique:
		handlingResult = CommonMessageHandler::handleControlMessage(msg);
		break;
	}

	return handlingResult;
}


HandlingResult SyncSlotMessageHandler::handleMasterSyncMessage(SyncMessage* msg){
	return handleSyncAspectOfSyncCarryingMsg(msg);
}

/*
OLD
Immediately join other clique.

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

	clique.assumeMastership();

	return HandlingResult::KeepListening;
}
