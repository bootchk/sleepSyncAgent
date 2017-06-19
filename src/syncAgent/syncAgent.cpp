
#include <cassert>
#include "syncAgent.h"
#include "globals.h"	// which includes nRF5x.h
#include "scheduleParameters.h"
#include "syncPowerSleeper.h"
#include "modules/syncPowerManager.h"
#include "modules/oversleepMonitor.h"




namespace {

// SyncAgent owns
SyncPowerSleeper syncPowerSleeper;
// SyncSleeper, Sleeper is global
}




// Static data members
// DYNAMIC uint8_t SyncAgent::receiveBuffer[255];

CliqueMerger SyncAgent::cliqueMerger;

void (*SyncAgent::onWorkMsgCallback)(WorkPayload);
void (*SyncAgent::onSyncPointCallback)();


// This file only implements part of the class, see other .cpp files.
// See syncAgentLoop.cpp for high level algorithm.



void SyncAgent::initSleepers(SyncPowerManager* aSyncPowerManager) {

	// assert longClockTimer was init
	// assert counter is perpetually running
	// assert counter interrupt enabled for overflow
	// assert RTC0_IRQ is enabled (for Counter overflow and any Timers)

	sleeper.init();
	sleeper.setSaneTimeout(ScheduleParameters::MaxSaneTimeoutSyncPowerSleeper);

	syncPowerManager = aSyncPowerManager;	// global

	// SyncSleeper, syncPowerSleeper is global, needs no init
}


void SyncAgent::sleepUntilSyncPower(){
	syncPowerSleeper.sleepUntilSyncPower();
}


void SyncAgent::initSyncObjects(
		Mailbox* aMailbox,
		SyncPowerManager* aSyncPowerManager,
		BrownoutManager* aBrownoutManager,
		void (*aOnWorkMsgCallback)(WorkPayload),
		void (*aOnSyncPointCallback)()
	)
{
	/*
	 * Require caller initialized radio, mailbox, and LongClockTimer.
	 * Require caller configured radio for protocol.
	 * !!! But not isOSClockRunning() yet.
	 * RADIO->POWER is set at POR reset, but it means 'was reset', not 'is using power'.
	 */

	// Copy parameters to globals
	workOutMailbox = aMailbox;
	syncPowerManager = aSyncPowerManager;

	// Temp: test power consumption when all sleep
	// while(true) waitForOSClockAndToRecoverBootEnergy(aLCT);

	// Copy parameters to static data members
	onWorkMsgCallback = aOnWorkMsgCallback;
	onSyncPointCallback = aOnSyncPointCallback;

	// Connect radio IRQ to syncSleeper so it knows reason for wake
	Ensemble::init(syncSleeper.getMsgReceivedCallback());

	// Serializer reads and writes directly to radio buffer
	serializer.init(Ensemble::getBufferAddress(), Radio::FixedPayloadCount);

	clique.init();

	// Register callbacks that return debug info
	aBrownoutManager->registerCallbacks(
			getPhase,
			getReasonForWake,
			OverSleepMonitor::timeSinceLastStartSleep);

	// assert LongClock is reset
	// not assert LongClock running assert(aLCT->isOSClockRunning());

	// ensure initial state of SyncAgent
	assert(role.isFisher());
	assert(clique.isSelfMaster());
	assert(!Ensemble::isRadioInUse());
	assert(Ensemble::isConfigured());
}






// Merger and Fisher are duals

void SyncAgent::toMergerRole(SyncMessage* msg){
	// assert slot is fishSlot
	assert(msg->carriesSync());
	assert(msg->type != MessageType::MergeSync);
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




void SyncAgent::relayHeardWorkToApp(WorkPayload work) {
	/*
	 * Alternatives are:
	 * - queue to worktask (unblock it)
	 * - onWorkMsgCallback(msg);  (callback)
	 *
	 * Here we do the latter: the app maintains a In queue, not SyncAgent.
	 * The app should do the work later, since now is at imprecise time is a slot.
	 */
	onWorkMsgCallback(work);	// call callback
}


/*
 * Callbacks from BrownoutManager for debugging.
 */
uint32_t SyncAgent::getPhase() { return (uint32_t) phase; }
uint32_t SyncAgent::getReasonForWake() { return (uint32_t) sleeper.getReasonForWake(); }




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
	msg->deltaToNextSyncPoint.set( clique.schedule.deltaFromWorkMiddleToEndSyncPeriod() ) ;
}
#endif
