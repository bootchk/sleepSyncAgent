
#include <cassert>
#include "syncAgent.h"
#include "globals.h"


// Static data members
bool SyncAgent::isSyncingState = false;
// DYNAMIC uint8_t SyncAgent::receiveBuffer[255];


CliqueMerger SyncAgent::cliqueMerger;
MergerFisherRole SyncAgent::role;

PowerManager SyncAgent::powerMgr;

void (*SyncAgent::onWorkMsgCallback)(WorkPayload);

LEDLogger SyncAgent::ledLogger;	// DEBUG

uint32_t SyncAgent::countValidReceives = 0;
uint32_t SyncAgent::countInvalidTypeReceives = 0;
uint32_t SyncAgent::countInvalidCRCReceives = 0;

// This file only implements part of the class, see other .cpp files.
// See syncAgentLoop.cpp for high level algorithm.



void SyncAgent::init(
		Radio * aRadio,
		void (*aOnWorkMsgCallback)(WorkPayload)
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

	onWorkMsgCallback = aOnWorkMsgCallback;

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



// Merger and Fisher are duals

void SyncAgent::toMergerRole(SyncMessage* msg){
	// msg received in fishSlot is MasterSync
	// msg received in syncSlot is Work (mangled)
	assert(msg->type == MasterSync || msg->type == Work);
	assert(role.isFisher());
	role.setMerger();
	cliqueMerger.initFromMsg(msg);

	// assert my schedule might have been adjusted
	// assert I might have relinquished mastership
	// assert I might have joined another clique
	assert(role.isMerger());
	ledLogger.toggleLED(3);
}


void SyncAgent::toFisherRole(){
	role.setFisher();
	// role does not know about cliqueMerger
	cliqueMerger.deactivate();
	ledLogger.toggleLED(3);
}

/*
 * Hack
 *
 * Fabricate an offset for a Work message, destroying previous offset.
 * Makes the Work message act as a MasterSync from its sender.
 *
 * !!! Work message offset field usually contains "WorkType".
 * Work message does contain a MasterID.
 * Since WorkSlot is in known time relationship with SyncSlot,
 * we can fabricate an offset to SyncSlot of sender of Work.
 */
void SyncAgent::mangleWorkMsg(SyncMessage* msg){
	// assert current slot is SyncSlot
	assert(msg->type = Work);
	/*
	 * assert msg was heard anywhere in SyncSlot (not just in the middle)
	 * but was sent from middle of sender's WorkSlot
	 */
	msg->deltaToNextSyncPoint = clique.schedule.deltaFromWorkMiddleToEndSyncPeriod() ;
}


void SyncAgent::relayWorkToApp(WorkPayload work) {
	/*
	 * Alternatives are:
	 * - queue to worktask (unblock it)
	 * - onWorkMsgCallback(msg);  (callback)
	 */
	onWorkMsgCallback(work);	// call callback
	// ledLogger.toggleLED(1);
}
