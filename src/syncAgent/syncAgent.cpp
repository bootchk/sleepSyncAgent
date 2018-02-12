
#include "syncAgent.h"

// Implementation
#include "../syncAgentImp/syncAgentImp.h"


// Each method simply delegates to the implementation SyncAgentImp

void SyncAgent::subscribeProvisioning(PropertyIndex propertyIndex, ProvisionCallback provisionCallback) {
		SyncAgentImp::subscribeProvisioning(propertyIndex, provisionCallback);
	}

void SyncAgent::initSyncObjects(
			Mailbox* mailbox,
			void (*onWorkMsg)(WorkPayload),
			void (*onSyncPoint)()
			) {
	SyncAgentImp::initSyncObjects(mailbox, onWorkMsg, onSyncPoint);
}

void SyncAgent::initSleepers() { SyncAgentImp::initSleepers(); }

void SyncAgent::initEnsembleProtocol() { SyncAgentImp::initEnsembleProtocol(); }

void SyncAgent::sleepUntilSyncPower() { SyncAgentImp::sleepUntilSyncPower(); }

void SyncAgent::loop() { SyncAgentImp::loop(); }

bool SyncAgent::isSelfMaster() { SyncAgentImp::isSelfMaster(); }
