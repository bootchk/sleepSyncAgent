
#include "syncSlot.h"

#include "syncSlotProperty.h"
#include "../../schedule/syncSchedule.h"

#include "../../network/intraCliqueManager.h"
#include "../../work/workOut.h"

#include "../../messageHandler/messageHandler.h"



void SyncSlot::beginListen() {
	Ensemble::startReceivingWithHandler(SyncSlotMessageHandler::handle);
	/*
	 * Two possible tasks:
	 * 1. Radio is listening. Events from radio handled by handlerTask.
	 * 2. Soon to be scheduled syncSlotEnd task
	 */
}



void SyncSlot::dispatchSyncSlotKind() {
	SyncSlotKind kind = SyncSlotProperty::decideKind();

	switch(kind) {
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
		break;
	}
	// Assert some task is scheduled
}

