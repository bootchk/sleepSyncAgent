
#include <cassert>
#include "syncAgent.h"



// Static data members
bool SyncAgent::isSyncingState = false;
// DYNAMIC uint8_t SyncAgent::receiveBuffer[255];

Clique SyncAgent::clique;
CliqueMerger SyncAgent::cliqueMerger;
MergePolicy SyncAgent::mergePolicy;
Role SyncAgent::role;

Serializer SyncAgent::serializer;
PowerManager SyncAgent::powerMgr;
Sleeper SyncAgent::sleeper;

Radio* SyncAgent::radio;
void (*SyncAgent::onWorkMsgQueuedCallback)();

LEDLogger SyncAgent::ledLogger;	// DEBUG

uint32_t SyncAgent::countValidReceives = 0;
uint32_t SyncAgent::countInvalidTypeReceives = 0;
uint32_t SyncAgent::countInvalidCRCReceives = 0;

// This file only implements part of the class, see many other .cpp files.
// See syncAgentLoop.cpp for high level algorithm.

void SyncAgent::init(
		Radio * aRadio,
		void (*aOnWorkMsgQueuedCallback)()
	)
{
	sleeper.init();
	// FUTURE hard to know who owns clock assert(sleeper.isOSClockRunning());

	radio = aRadio;

	// Connect radio IRQ to sleeper so it knows reason for wake
	radio->init(&sleeper.msgReceivedCallback);
	// radio not configured until after powerOn()

	// Serializer reads and writes directly to radio buffer
	serializer.init(radio->getBufferAddress(), Radio::FixedPayloadCount);

	onWorkMsgQueuedCallback = aOnWorkMsgQueuedCallback;

	clique.reset();
	// Assert LongClock is reset and running

	// radio device may be on from prior debugging w/o hard reset
	radio->powerOff();

	// ensure initial state of SyncAgent
	assert(role.isFisher());
	assert(clique.isSelfMaster());
	assert(!radio->isPowerOn());
}


void SyncAgent::pauseSyncing() {
	/*
	 * Not enough power for self to continue syncing.
	 * Other units might still have power and assume mastership of my clique
	 */

	assert(!radio->isPowerOn());

	// FUTURE if clique is probably not empty
	if (clique.isSelfMaster()) doDyingBreath();
	// else I am a slave, just drop out of clique, others may have enough power

	// FUTURE onSyncingPausedCallback();	// Tell app
}


void SyncAgent::doDyingBreath() {
	// Ask another unit in my clique to assume mastership.
	// Might not be heard.
	serializer.outwardCommonSyncMsg.makeAbandonMastership(myID());
	// assert common SyncMessage serialized into radio buffer
	radio->transmitStaticSynchronously();	// blocks until transmit complete
}


