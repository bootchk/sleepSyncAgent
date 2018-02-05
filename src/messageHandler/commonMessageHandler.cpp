
#include "messageHandler.h"

#include "../logging/logger.h"
#include "../network/controller.h"

/*
 * No matter what slot heard in,
 * handled the same,
 * and keep listening.
 */
HandlingResult CommonMessageHandler::handleInfoMessage(SyncMessage* msg){
	Logger::logReceivedInfo(msg->work);
	return HandlingResult::KeepListening;
}


HandlingResult CommonMessageHandler::handleControlMessage(SyncMessage* msg){
	Controller::setXmitPower(msg->work);
	return HandlingResult::KeepListening;
}
