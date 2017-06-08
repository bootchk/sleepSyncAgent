
#include "messageHandler.h"

#include "../logMessage.h"

/*
 * Dispatcher common to all subclasses of MessageHandler
 */
HandlingResult MessageHandler::handle(SyncMessage* msg){
	HandlingResult handlingResult;

	switch(msg->type) {
	case MessageType::MasterSync:
		log(LogMessage::RXMasterSync);
		handlingResult = handleMasterSyncMessage(msg);
		break;
	case MessageType::MergeSync:
		log(LogMessage::RXMergeSync);
		handlingResult = handleMergeSyncMessage(msg);
		break;
	case MessageType::AbandonMastership:
		log(LogMessage::RXAbandonMastership);
		handlingResult = handleAbandonMastershipMessage(msg);
		break;
	case MessageType::WorkSync:
		log(LogMessage::RXWorkSync);
		handlingResult = handleWorkSyncMessage(msg);
		break;
	}

	return handlingResult;
}





