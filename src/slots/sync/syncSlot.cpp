
#include "syncSlot.h"

#include "syncSlotProperty.h"
#include "../../schedule/syncSchedule.h"

#include "../../network/intraCliqueManager.h"
#include "../../work/workOut.h"

#include "../../messageHandler/messageHandler.h"
#include "../../receiver/receiver.h"

#include "../../globals.h"	// clique
#include "../../clique/clique.h"

#include "../../syncAgentImp/syncAgentImp.h"
#include "../../syncAgentImp/state/syncMode.h"


namespace {
	/*
	 * State of sync slot: once we decide, we remember;
	 */
	SyncSlotKind _kind;
}




SyncSlotKind SyncSlot::kind() { return _kind; }




/*
 * Do least possible, since it shortens the sync listen.
 *
 * Older design calledBack app and transitioned mode here.
 * Now, that is moved to the RadioPrelude slot before the SyncSlot.
 */
void SyncSlot::bookkeepingAtStartSyncSlot() {
	SyncAgentImp::preludeToSyncPeriod();
}


void SyncSlot::bookkeepingAtPreludeToSyncSlot() {
	SyncAgentImp::callbackAppPreSync();
	SyncModeManager::checkPowerAndTryModeTransitions();
}



void SyncSlot::beginListen() {
	Logger::log(" Listen ");
	Receiver::startWithHandler(SyncSlotMessageHandler::handle);
}

// Some duplication w old SyncWorkSlot::endListen
void SyncSlot::endListen() {
	// Radio might be in use if we timeout'd while receiving
	Receiver::stop();

	// FUTURE we could do this elsewhere, e.g. start of sync slot
	if (!clique.isSelfMaster())
		clique.checkMasterDroppedOut();
}



// TODO slots/syncing/?  endListen() is old and not for RTC Task


void SyncSlot::dispatchSyncSlotKind() {
	_kind = SyncSlotProperty::decideKind();

	switch(_kind) {
	case SyncSlotKind::sendControlSync:
		/*
		 * For all these cases:
		 * satisfies needXmitSync (since Sync, WorkSync, ControlSync all carry sync
		 * RadioPrelude::isDone.
		 * assert(not Radio::isActive());
		 * Sleep until time to transmit.
		 */
		SyncSchedule::syncSendTask();
		// TODO what task does this get done, before or after
		IntraCliqueManager::checkDoneAndEnactControl();
		break;
		// TODO merge case with below cases
	case SyncSlotKind::sendWorkSync:
	case SyncSlotKind::sendSync:
		SyncSchedule::syncSendTask();
		break;
	case SyncSlotKind::listen:
		// isSlave OR (isMaster and not xmitting (coin flip))

		beginListen();
		SyncSchedule::syncSlotEndFromListen();
		/*
		 * Two possible tasks:
		 * 1. Radio is listening. Events from radio handled by handlerTask.
		 * 2. Scheduled syncSlotEnd task
		 */
		break;
	}
	// Assert some task is scheduled
}

