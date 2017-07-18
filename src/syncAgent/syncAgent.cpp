
#include <cassert>
#include "syncAgent.h"
#include "globals.h"	// which includes nRF5x.h
#include "scheduleParameters.h"
#include "sleepers/syncPowerSleeper.h"
#include "sleepers/syncSleeper.h"
#include "sleepers/oversleepMonitor.h"
//#include "modules/syncPowerManager.h"
#include "message/messageFactory.h"
#include "message/serializer.h"
#include "state/phase.h"



// SyncSleeper, Sleeper pure classes


// Static data members
// DYNAMIC uint8_t SyncAgent::receiveBuffer[255];

CliqueMerger SyncAgent::cliqueMerger;

WorkPayload SyncAgent::countMergeSyncHeard;

void (*SyncAgent::onWorkMsgCallback)(WorkPayload);
void (*SyncAgent::onSyncPointCallback)();


// This file only implements part of the class, see other .cpp files.
// See syncAgentLoop.cpp for high level algorithm.



void SyncAgent::initSleepers() {

	// assert longClock was init
	// assert counter is perpetually running
	// assert counter interrupt enabled for overflow
	// assert RTC0_IRQ is enabled (for Counter overflow and any Timers)

	Sleeper::setSaneTimeout(ScheduleParameters::TimeoutWaitingForSyncPowerSleeper);

	// SyncSleeper, syncPowerSleeper is global, needs no init
}


void SyncAgent::sleepUntilSyncPower(){
	SyncPowerSleeper::sleepUntilSyncPower();
}


void SyncAgent::initSyncObjects(
		Mailbox* aMailbox,
		void (*aOnWorkMsgCallback)(WorkPayload),
		void (*aOnSyncPointCallback)()
	)
{
	/*
	 * Require caller initialized radio, mailbox, and LongClock.
	 * Require caller configured radio for protocol.
	 * !!! But not isOSClockRunning() yet.
	 * RADIO->POWER is set at POR reset, but it means 'was reset', not 'is using power'.
	 */

	// Copy parameters to globals
	workOutMailbox = aMailbox;

	// Temp: test power consumption when all sleep
	// while(true) waitForOSClockAndToRecoverBootEnergy(aLCT);

	// Copy parameters to static data members
	onWorkMsgCallback = aOnWorkMsgCallback;
	onSyncPointCallback = aOnSyncPointCallback;

	/*
	 * Initialize members (Radio, HfClock, DCDC) of ensemble.
	 * Connect radio IRQ to syncSleeper so it knows reason for wake
	 */
	Ensemble::init(SyncSleeper::getMsgReceivedCallback());

	// Serializer reads and writes directly to radio buffer
	Serializer::init(Ensemble::getBufferAddress(), Radio::FixedPayloadCount);

	clique.init();

	// Register callbacks that return debug info
	/*
	 * BrownoutRecorder will also record phase if not already written by prior oversleep.
	 */
	BrownoutRecorder::registerCallbacks(
			//getPhase,
			SyncSleeper::getCountSleeps,
			//SyncSleeper::getPriorReasonForWake,
			OverSleepMonitor::getPhaseAndReason,
			OverSleepMonitor::timeElapsedSinceLastStartSleep);

	countMergeSyncHeard = 0;

	// assert LongClock is reset
	// not assert LongClock running assert(aLCT->isOSClockRunning());

	// ensure initial state of SyncAgent
	assert(MergerFisherRole::isFisher());
	assert(clique.isSelfMaster());
	assert(!Ensemble::isRadioInUse());
	assert(Ensemble::isConfigured());
}






// Merger and Fisher are duals

/*
 * We fished another clique.
 * Begin MergerRole
 */
void SyncAgent::toMergerRole(SyncMessage* msg){
	// assert slot is fishSlot
	assert (MessageFactory::carriesSync(msg->type));
	assert(msg->type != MessageType::MergeSync);
	assert(MergerFisherRole::isFisher());
	MergerFisherRole::setMerger();
	// logging done later
	cliqueMerger.initFromMsg(msg);

	// assert my schedule might have been adjusted
	// assert I might have relinquished mastership
	// assert I might have joined another clique
	// assert(role.isMerger());
}


void SyncAgent::toFisherRole(){
	MergerFisherRole::setFisher();
	// role does not know about cliqueMerger
	cliqueMerger.deactivate();
	Logger::log(Logger::ToFisher);
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
uint32_t SyncAgent::getPhase() { return (uint32_t) Phase::get(); }
uint32_t SyncAgent::getReasonForWake() { return (uint32_t) Sleeper::getReasonForWake(); }




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
