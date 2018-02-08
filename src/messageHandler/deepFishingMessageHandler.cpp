
#include <cassert>

#include "messageHandler.h"

#include "../clique/clique.h"
#include "../globals.h"

#include "../slots/fishing/fishingManager.h"


/*
 * Another unit said a superior exists where we are deep fishing
 * We might have caught it.
 */
HandlingResult FishSlotMessageHandler::handleCatchFromDeepFishing(SyncMessage* msg){
	HandlingResult result;

	assert (SyncMessage::doesCarrySync(msg->type));
	if (clique.isOtherCliqueBetter(msg->masterID)) {
		// Intended result of deep fishing: found a superior clique.

		// Join other clique
		clique.updateBySyncMessage(msg);

		// Abort deep fishing
		FishingManager::switchToTrolling();

		// Abort slot, it succeeded
		result = HandlingResult::StopListeningHeardMasterSync;
	}
	else { // Inferior
		// Unintended result
		// Ignore msg, continue fish slot session and keep deep fishing for superior clique
		result = HandlingResult::KeepListening;
	}
	// Schedule and FishingMode probably changed

	return result;
}
