
#include "syncSlot.h"

#include "syncSlotProperty.h"
#include "../../schedule/syncSchedule.h"

#include "../../network/intraCliqueManager.h"
#include "../../work/workOut.h"

#include "../../messageHandler/messageHandler.h"
#include "../../radio/radio.h"


namespace {
	/*
	 * State of sync slot: once we decide, we remember;
	 */
	SyncSlotKind _kind;
}




SyncSlotKind SyncSlot::kind() { return _kind; }



void SyncSlot::beginListen() {
	Radio2::startReceivingWithHandler(SyncSlotMessageHandler::handle);
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
		//Logger::log(" Listens ");
		SyncSchedule::syncSlotEndListen();
		/*
		 * Two possible tasks:
		 * 1. Radio is listening. Events from radio handled by handlerTask.
		 * 2. Scheduled syncSlotEnd task
		 */
		break;
	}
	// Assert some task is scheduled
}

