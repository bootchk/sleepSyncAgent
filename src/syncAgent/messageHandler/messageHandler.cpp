
#include "messageHandler.h"

#include "../logMessage.h"

HandlingResult MessageHandler::handle(SyncMessage* msg){
	HandlingResult handlingResult;

	switch(msg->type) {
	case MasterSync:
		log(LogMessage::RXMasterSync);
		handlingResult = handleMasterSyncMessage(msg);
		break;
	case MergeSync:
		log(LogMessage::RXMergeSync);
		handlingResult = handleMergeSyncMessage(msg);
		break;
	case AbandonMastership:
		log(LogMessage::RXAbandonMastership);
		handlingResult = handleAbandonMastershipMessage(msg);
		break;
	case WorkSync:
		log(LogMessage::RXWorkSync);
		handlingResult = handleWorkSyncMessage(msg);
		break;
	default:
		log(LogMessage::RXUnknown);
		handlingResult = HandlingResult::KeepListening;
	}

	return handlingResult;
}





