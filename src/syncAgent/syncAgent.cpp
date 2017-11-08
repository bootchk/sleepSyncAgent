
#include <cassert>
#include "syncAgent.h"
#include "globals.h"	// clique
#include "cliqueHistory/cliqueHistory.h"
#include "scheduleParameters.h"
#include "sleepers/syncPowerSleeper.h"
#include "sleepers/syncSleeper.h"
#include "sleepers/oversleepMonitor.h"
#include "message/serializer.h"
#include "state/phase.h"
#include "state/role.h"

#include "policy/workManager.h"
#include "logging/logger.h"


// SyncSleeper, Sleeper pure classes


// Static data members
// DYNAMIC uint8_t SyncAgent::receiveBuffer[255];

CliqueMerger SyncAgent::cliqueMerger;

// A statistic we can include in certain messages
WorkPayload SyncAgent::countMergeSyncHeard;

void (*SyncAgent::onWorkMsgCallback)(WorkPayload);
void (*SyncAgent::onSyncPointCallback)();

RadioUseCase radioUseCaseSleepSync;


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

	MergerFisherRole::init();
	WorkManager::init(aMailbox);

	// Temp: test power consumption when all sleep
	// while(true) waitForOSClockAndToRecoverBootEnergy(aLCT);

	// Copy parameters to static data members
	onWorkMsgCallback = aOnWorkMsgCallback;
	onSyncPointCallback = aOnSyncPointCallback;

	/*
	 * Initialize members (Radio, HfClock, DCDC) of ensemble.
	 * Connect radio IRQ to syncSleeper so it knows reason for wake
	 */
	Ensemble::init(SyncSleeper::getMsgReceivedCallback(), &radioUseCaseSleepSync);

#ifdef LOW_XMIT_POWER
	// TEMP: testing range with lower xmit power
	// Plus0, Minus4, Minus12, Minus40
	radioUseCaseSleepSync.setXmitPower(TransmitPowerdBm::Minus40);
#else
	// Default: 0dBm
#endif

	// Serializer reads and writes directly to radio buffer
	Serializer::init(Ensemble::getBufferAddress(), Radio::FixedPayloadCount);

	clique.init();
	CliqueHistory::init();

	/*
	 * Register callbacks that return debug info
	 * BrownoutRecorder will also record phase if not already written by prior oversleep.
	 *
	 * Some choices commented out.
	 */
	BrownoutRecorder::registerCallbacks(
			//getPhase,
			//SyncSleeper::getCountSleeps,
			Logger::logBrownout,
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



bool SyncAgent::isSelfMaster() { return clique.isSelfMaster(); }


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
