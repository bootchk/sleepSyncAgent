
#include "messageHandler.h"

#include "../logMessage.h"

HandlingResult MessageHandler::handle(SyncMessage* msg){
	HandlingResult foundDesiredMessage;

	switch(msg->type) {
	case MasterSync:
		log(LogMessage::RXMasterSync);
		foundDesiredMessage = handleMasterSyncMessage(msg);
		break;
	case MergeSync:
		log(LogMessage::RXMergeSync);
		foundDesiredMessage = handleMergeSyncMessage(msg);
		break;
	case AbandonMastership:
		log(LogMessage::RXAbandonMastership);
		foundDesiredMessage = handleAbandonMastershipMessage(msg);
		break;
	case WorkSync:
		log(LogMessage::RXWorkSync);
		foundDesiredMessage = handleWorkSyncMessage(msg);
		break;
	default:
		log(LogMessage::RXUnknown);
		foundDesiredMessage = KeepListening;
	}

	return foundDesiredMessage;
}





