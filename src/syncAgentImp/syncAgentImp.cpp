
#include "../syncAgentImp/syncAgentImp.h"

#include <cassert>
#include "../globals.h"	// clique
#include "../cliqueHistory/cliqueHistory.h"
#include "../scheduleParameters.h"
#include "../sleepers/syncPowerSleeper.h"
#include "../sleepers/syncSleeper.h"
#include "../sleepers/oversleepMonitor.h"
#include "../message/serializer.h"

#ifdef SOFTDEVICE_PRESENT
// Provisioning
#include "../network/topology.h"
#include "../provisioning/workProvisioningProxy.h"
#endif


#include "../logging/logger.h"
#include "../syncAgentImp/state/phase.h"
#include "../syncAgentImp/state/syncMode.h"
#include "../work/workOut.h"
#include "../work/workIn.h"


// SyncSleeper, Sleeper pure classes


// Static data members
// DYNAMIC uint8_t SyncAgentImp::receiveBuffer[255];

CliqueMerger SyncAgentImp::cliqueMerger;

// A statistic we can include in certain messages
unsigned char SyncAgentImp::countMergeSyncHeard;

RadioUseCase radioUseCaseSleepSync;


// This file only implements part of the class, see other .cpp files.
// See syncAgentLoop.cpp for high level algorithm.



void SyncAgentImp::initSleepers() {

	// assert longClock was init
	// assert counter is perpetually running
	// assert counter interrupt enabled for overflow
	// assert RTCx_IRQ is enabled (for Counter overflow and any Timers)

	Sleeper::setSaneTimeout(ScheduleParameters::TimeoutWaitingForSyncPowerSleeper);

	// SyncSleeper, syncPowerSleeper is global, needs no init
}


void SyncAgentImp::sleepUntilSyncPower(){
	SyncPowerSleeper::sleepUntilSyncPower();
}


void SyncAgentImp::initSyncObjects(
		Mailbox* aMailbox,
		void (*aOnWorkMsgCallback)(unsigned char),
		void (*aOnSyncPointCallback)()
	)
{
	/*
	 * Require caller initialized radio, mailbox, and LongClock.
	 * Require caller configured radio for protocol.
	 * !!! But not isOSClockRunning() yet.
	 * RADIO->POWER is set at POR reset, but it means 'was reset', not 'is using power'.
	 */

	SyncModeManager::resetToModeMaintain();
	WorkOut::init(aMailbox);

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

	initEnsembleProtocol();

#ifdef SOFTDEVICE_PRESENT
	NetworkTopology::subscribeToProvisionings();
	WorkProvisioningProxy::subscribeToProvisionings();
#endif

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

	// ensure initial state of SyncAgentImp
	// assert(MergerFisherRole::isFisher());
	assert(clique.isSelfMaster());
	assert(!Ensemble::isRadioInUse());
	assert(Ensemble::isConfigured());
}



bool SyncAgentImp::isSelfMaster() { return clique.isSelfMaster(); }
void SyncAgentImp::checkMasterDroppedOut() { return clique.checkMasterDroppedOut(); }
SystemID SyncAgentImp::getMasterID() { return clique.getMasterID(); }

void SyncAgentImp::scatter() {
	/*
	 * Reset to initial conditions.
	 * In general, reset all state, all policies.
	 *
	 * Abort any merging in progress.
	 * Abort any work messages received.
	 */

	/*
	 * Clique resets several monitors and policies.
	 */
	clique.scatterSync();

	// Mode and submode not fish/merging
	SyncModeManager::resetToModeMaintain();

	// scatter work exchange protocol???? TODO
	WorkIn::resetState();

	/*
	 * The app, if it is working autonomously (but in sync with other work)
	 * will now work out of sync, since clique schedule is scattered.
	 * We don't need to tell the app to scatter its work.
	 */

	/*
	 * Not reset:
	 * - the clique history???
	 * - network topology
	 */

	// TODO assertions on state of all policies and other state
}


/*
 * Callbacks from BrownoutManager for debugging.
 */
uint32_t SyncAgentImp::getPhase() { return (uint32_t) Phase::get(); }
uint32_t SyncAgentImp::getReasonForWake() { return (uint32_t) Sleeper::getReasonForWake(); }


void SyncAgentImp::initEnsembleProtocol() {
	/*
	 * Only the radio needs configuration.
	 * The LF clock is always running.
	 */
	// For now, setting means apply to
	// And it means configure radio prototocol, even though radioUseCase doesn't know it
	Ensemble::setRadioUseCase(&radioUseCaseSleepSync);
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
void SyncAgentImp::mangleWorkMsg(SyncMessage* msg){
	// assert current slot is SyncSlot
	assert(msg->type = WorkSync);
	/*
	 * assert msg was heard anywhere in SyncSlot (not just in the middle)
	 * but was sent from middle of sender's WorkSlot
	 */
	msg->deltaToNextSyncPoint.set( clique.schedule.deltaFromWorkMiddleToEndSyncPeriod() ) ;
}
#endif
