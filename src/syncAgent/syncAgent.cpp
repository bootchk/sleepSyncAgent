
#include "syncAgent.h"

// Implementation
#include "../syncAgentImp/syncAgentImp.h"


// Each method simply delegates to the implementation SyncAgentImp

void SyncAgent::initSyncObjects(
			Mailbox* mailbox,
			void (*onWorkMsg)(unsigned char),
			void (*onSyncPoint)()
			) {
	SyncAgentImp::initSyncObjects(mailbox, onWorkMsg, onSyncPoint);
}

void SyncAgent::initSleepers() { SyncAgentImp::initSleepers(); }

void SyncAgent::initEnsembleProtocol() { SyncAgentImp::initEnsembleProtocol(); }

void SyncAgent::sleepUntilSyncPower() { SyncAgentImp::sleepUntilSyncPower(); }

void SyncAgent::loop() { SyncAgentImp::loop(); }

bool SyncAgent::isSelfMaster() { return SyncAgentImp::isSelfMaster(); }
bool SyncAgent::isSelfSlave() { return ! isSelfMaster(); }
