
#include "messageHandler.h"

#include "../logging/logger.h"
#include "../network/topology.h"
#include "../provisioning/workProvisioningProxy.h"




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
	NetworkTopology::handleNetGranularityMessage(msg);
	return HandlingResult::KeepListening;
}

HandlingResult CommonMessageHandler::handleControlMessageScatter(SyncMessage* msg){
	NetworkTopology::handleScatterMessage(msg);
	return HandlingResult::KeepListening;
}




HandlingResult CommonMessageHandler::handleControlMessageWorkTime(SyncMessage* msg){
	WorkProvisioningProxy::handleWorkTimeMessage(msg);
	return HandlingResult::KeepListening;
}

HandlingResult CommonMessageHandler::handleControlMessageWorkCycle(SyncMessage* msg){
	WorkProvisioningProxy::handleWorkCycleMessage(msg);
	return HandlingResult::KeepListening;
}
