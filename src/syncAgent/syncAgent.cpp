
#include <cassert>
#include "syncAgent.h"
#include "globals.h"	// which includes nRF5x.h
#include "scheduleParameters.h"


// Static data members
bool SyncAgent::isSyncingState = false;
// DYNAMIC uint8_t SyncAgent::receiveBuffer[255];

CliqueMerger SyncAgent::cliqueMerger;

void (*SyncAgent::onWorkMsgCallback)(WorkPayload);
void (*SyncAgent::onSyncPointCallback)();


// This file only implements part of the class, see other .cpp files.
// See syncAgentLoop.cpp for high level algorithm.


namespace {

/*
 * Sleep (not spin!!!) to recover boot energy and to insure LFXO is stable (takes 0.25 seconds.)
 * Necessary when power is load switched and hysteresis is low (.05V) and power storage is small capacitor.
 */
void waitForOSClockAndToRecoverBootEnergy(LongClockTimer * aLCT) {
	// Init sleeper with a larger timeout limit than while syncing
	syncSleeper.init(
			ScheduleParameters::StabilizedClockTimeout + 1,
			aLCT);

	syncSleeper.sleepUntilTimeout(ScheduleParameters::StabilizedClockTimeout);
}

}


void SyncAgent::init(
		Radio * aRadio,
		Mailbox* aMailbox,
		LongClockTimer * aLCT,
		void (*aOnWorkMsgCallback)(WorkPayload),
		void (*aOnSyncPointCallback)()
	)
{
	// require caller initialized radio, mailbox, and LongClockTimer
	// !!! But not isOSClockRunning() yet

	// Copy parameters to globals
	radio = aRadio;
	workOutMailbox = aMailbox;

	// Radio is powered on at POR reset!!!
	radio->powerOff();

	// Temp: test power consumption when all sleep
	// while(true) waitForOSClockAndToRecoverBootEnergy(aLCT);

	waitForOSClockAndToRecoverBootEnergy(aLCT);

	assert(aLCT->isOSClockRunning());

	syncSleeper.init(
			2* ScheduleParameters::NormalSyncPeriodDuration,
			aLCT);

	// Copy parameters to static data members
	onWorkMsgCallback = aOnWorkMsgCallback;
	onSyncPointCallback = aOnSyncPointCallback;

	// Connect radio IRQ to syncSleeper so it knows reason for wake
	radio->setMsgReceivedCallback(syncSleeper.getMsgReceivedCallback());
	// radio not configured until after powerOn()

	// Serializer reads and writes directly to radio buffer
	serializer.init(radio->getBufferAddress(), Radio::FixedPayloadCount);

	clique.init();
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
	syncSender.sendAbandonMastership();
}



// Merger and Fisher are duals

void SyncAgent::toMergerRole(SyncMessage* msg){
	// assert slot is fishSlot
	assert(msg->carriesSync());
	assert(msg->type != MergeSync);
	assert(role.isFisher());
	role.setMerger();
	cliqueMerger.initFromMsg(msg);

	// assert my schedule might have been adjusted
	// assert I might have relinquished mastership
	// assert I might have joined another clique
	assert(role.isMerger());
	log(LogMessage::ToMerger);
}


void SyncAgent::toFisherRole(){
	role.setFisher();
	// role does not know about cliqueMerger
	cliqueMerger.deactivate();
	log(LogMessage::ToFisher);
}




void SyncAgent::relayWorkToApp(WorkPayload work) {
	/*
	 * Alternatives are:
	 * - queue to worktask (unblock it)
	 * - onWorkMsgCallback(msg);  (callback)
	 */
	onWorkMsgCallback(work);	// call callback
}


#ifdef OBSOLETE
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
#endif
