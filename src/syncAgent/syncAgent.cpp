
#include <cassert>
#include "syncAgent.h"



// Static data members
bool SyncAgent::isSyncingState = false;
// DYNAMIC uint8_t SyncAgent::receiveBuffer[255];

Clique SyncAgent::clique;
DropoutMonitor SyncAgent::dropoutMonitor;
CliqueMerger SyncAgent::cliqueMerger;
MergePolicy SyncAgent::mergePolicy;
Role SyncAgent::role;

Serializer SyncAgent::serializer;
PowerManager SyncAgent::powerMgr;
Sleeper SyncAgent::sleeper;

Radio* SyncAgent::radio;
void (*SyncAgent::onWorkMsgQueuedCallback)();

LEDLogger SyncAgent::ledLogger;	// DEBUG


// This file only implements part of the class, see many other .cpp files.
// See syncAgentLoop.cpp for high level algorithm.

void SyncAgent::init(
		Radio * aRadio,
		void (*aOnWorkMsgQueuedCallback)()
	)
{
	sleeper.init();
	// Assert OSClock is running

	radio = aRadio;
	// Configure radio
	// Connect radio IRQ to sleeper so it knows reason for wake
	radio->init(&sleeper.msgReceivedCallback);

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
	assert(sleeper.isOSClockRunning());
}

