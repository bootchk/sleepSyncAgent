
#include "messageHandler.h"

#include "../globals.h"  // clique

#include "../logging/logger.h"
#include "../network/topology.h"

#include "../clique/clique.h"



/*
 * No matter what slot heard in,
 * handled the same,
 * and keep listening.
 */
HandlingResult CommonMessageHandler::handleInfoMessage(SyncMessage* msg){
	Logger::logReceivedInfo(msg->work);
	return HandlingResult::KeepListening;
}


HandlingResult CommonMessageHandler::handleControlMessageSetGranularity(SyncMessage* msg){
	if (clique.isMsgFromMyClique(msg->masterID)) {
		NetworkTopology::setGranularity(msg);
	}
	return HandlingResult::KeepListening;
}

HandlingResult CommonMessageHandler::handleControlMessageScatter(SyncMessage* msg){
	// TODO
	return HandlingResult::KeepListening;
}
