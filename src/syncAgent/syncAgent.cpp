
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
void (*SyncAgent::onSyncPointCallback)();

LEDLogger SyncAgent::ledLogger;	// DEBUG

uint32_t countValidReceives = 0;
uint32_t countInvalidTypeReceives = 0;
uint32_t countInvalidCRCReceives = 0;


SyncSlot SyncAgent::syncSlot;
WorkSlot SyncAgent::workSlot;
FishSlot SyncAgent::fishSlot;
MergeSlot SyncAgent::mergeSlot;





// This file only implements part of the class, see other .cpp files.
// See syncAgentLoop.cpp for high level algorithm.



void SyncAgent::init(
		Radio * aRadio,
		Mailbox* aMailbox,
		void (*aOnWorkMsgCallback)(WorkPayload),
		void (*aOnSyncPointCallback)()
	)
{
	syncSleeper.init();
	// FUTURE hard to know who owns clock assert(sleeper.isOSClockRunning());

	// Copy parameters to globals
	radio = aRadio;
	workOutMailbox = aMailbox;

	// Copy parameters to static data members
	onWorkMsgCallback = aOnWorkMsgCallback;
	onSyncPointCallback = aOnSyncPointCallback;

	// Connect radio IRQ to syncSleeper so it knows reason for wake
	radio->init(syncSleeper.getMsgReceivedCallback());
	// radio not configured until after powerOn()

	// Serializer reads and writes directly to radio buffer
	serializer.init(radio->getBufferAddress(), Radio::FixedPayloadCount);

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


/*
 * Ask an other unit in my clique to assume mastership.
 * Might not be heard, in which case other units should detect DropOut.
 */
void SyncAgent::doDyingBreath() {
	serializer.outwardCommonSyncMsg.makeAbandonMastership(myID());
	syncSender.sendPrefabricatedMessage();
}



// Merger and Fisher are duals

void SyncAgent::toMergerRole(SyncMessage* msg){
	// msg received in fishSlot is MasterSync
	// msg received in syncSlot is Work (mangled)
	assert(msg->type == MasterSync || msg->type == WorkSync);
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
	assert(msg->type = WorkSync);
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
