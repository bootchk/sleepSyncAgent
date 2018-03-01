
#include "syncWorkSlot.h"

#include "../../modules/syncBehaviour.h"
#include "../../policy/workManager.h"
#include "../../network/intraCliqueManager.h"

#include "syncSlotSchedule.h"
#include "../../modules/syncSender.h"
#include "../../logging/logger.h"
#include "../../sleepers/syncSleeper.h"

#include <cassert>
#include "../../syncAgentImp/state/phase.h"

/*
 * Sub types of syncWorkSlot
 *
 * Behavior varies according to what needs to be sent.
 */


void SyncWorkSlot::dispatchSyncSlotKind() {

	/*
	 * Call shouldTransmitSync each sync slot: it ticks an alarming clock.
	 * Result is true if alarm, meaning: need xmit sync to prevent long gaps in sync transmittals.
	 */
	bool needXmitSync = SyncBehaviour::shouldTransmitSync();

	/*
	 * Order is important: priority: Control, work, regular sync.
	 */

	// Must call shouldSendControlSync to count down
	if (IntraCliqueManager::IntraCliqueManager::shouldSendControlSync()) {
		// ControlSync satisfies needXmitSync
		doSendingControlSyncWorkSlot();
		IntraCliqueManager::checkDoneAndEnactControl();
	}
	/*
	 * Work is higher priority than ordinary sync.
	 * Work must be rare, lest it flood airwaves and destroy sync.
	 * (colliding too often with MergeSync or MasterSync.)
	 *
	 * App (user of SyncAgent) decides policy and conveys  (queues) work to be sent.
	 */
	else if (WorkManager::isNeedSendWork()) {
		// This satisfies needXmitSync
		doSendingWorkSyncWorkSlot();
	}
	else {
		// No work to send, maintain sync if master
		if (needXmitSync)
			doMasterSyncWorkSlot();
		else {
			/*
			 * isSlave
			 * OR (isMaster and not xmitting (coin flip))
			 */
			doSlaveSyncWorkSlot();
			Logger::log(" Listens ");
		}
	}
	/*
	 * Assert some slot was done, either with a transmit or just listen.
	 */
}


void SyncWorkSlot::doSendingControlSyncWorkSlot() {
	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot);

	// SyncSender accesses IntraCliqueManager for message
	SyncSender::sendControlSync();

	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotEnd);
}



/*
 * Transmit WorkSync in middle.
 * App has queued work to be sent.
 */
void SyncWorkSlot::doSendingWorkSyncWorkSlot(){
	// not assert self is Master

	Phase::set(PhaseEnum::SyncListenFirstHalf);
	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot);
	assert(!Ensemble::isRadioInUse());

	/*
	 * Even if I heard sync, need send Work.
	 * assert self is Master or Slave.
	 * Might have heard sync from:
	 * - worse clique
	 * - master
	 * - better clique (and self if Master might have relinquished mastership.)
	 *
	 * Even if my clique changed, need to send workSync to it.
	 */

	/*
	 * Design choice: all work is equivalent: don't send if others sent.
	 *
	 * Also, this knows that we bounce out work back to app.
	 */
	// Fetch work from app
	MailContents work = WorkManager::fetch();
	if (! WorkManager::isHeardWork() ) {
		Phase::set(PhaseEnum::SyncXmitWorkSync);
		SyncSender::sendWorkSync(work);
		/*
		 * App sent this work.  App also queues it in to itself if app wants to work as well as tell others to work.
		 */
	}
	else {
		/*
		 * Someone else sent work (which carries sync)
		 * so I don't send any sync at all.
		 */
		Logger::log("Heard work, omit send work.\n");
		// assert heard work is relayed to app via callback
		/*
		 * FUTURE: work is distinct and mailbox holds many work
		 * send distinct work
		 * and queue another one from app back to app
		 */
	}


	/*
	 * Keep listening for other better Masters or WorkSync.
	 *
	 * Even if I heard a sync, I also sent one (to convey work.)
	 * I could still hear my Master, or other work.
	 *
	 * Result doesn't matter, slot is over and we proceed regardless whether we heard sync keeping msg.
	 */
	Phase::set(PhaseEnum::SyncListenSecondHalf);
	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotEnd);

	// not assert Ensemble::isLowPower()
}



/*
 * listen for sync the whole period.
 */
void SyncWorkSlot::doSlaveSyncWorkSlot() {


	//LogMessage::logListenFullSlot();
	Ensemble::startReceiving();

	// Assert listening for other's sync.
	// Cannot assert isRadioInUse() since receive might have succeeded already (when using debugger)

	// Log delay from sync point to actual start listening.
	// logInt(clique.schedule.deltaPastSyncPointToNow()); log("<delta SP to sync listen.\n");

	Phase::set(PhaseEnum::SyncSlaveListen);
	(void) SyncSleeper::sleepUntilMsgAcceptedOrTimeout(
			SyncSlotMessageHandler::handle,
			SyncSlotSchedule::deltaToThisSyncSlotEnd);
	/*
	 * Not using result:  all message handlers return false i.e. keep looking.
	 * Assert we timed out and now is end of slot.
	 */
}



/*
 * Transmit any sync in middle of slot.
 */
void SyncWorkSlot::doMasterSyncWorkSlot() {

	HandlingResult handlingResult;

	Phase::set(PhaseEnum::SyncListenFirstHalf);
	handlingResult = doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot);
	assert(!Ensemble::isRadioInUse());

	/*
	 * Moment to xmit sync.
	 * But only if we didn't already (in first half) hear a message that keeps sync for our clique.
	 * Self was Master, but a heard sync might have changed that.
	 */
	if ( ! didFirstHalfHearSync(handlingResult)) {
		/*
		 * Self is still master.
		 */
		Phase::set(PhaseEnum::SyncXmit);
		SyncSender::sendMasterSync();
	}

	/*
	 * Keep listening for other better Masters that might have fished us (MergeSync) and WorkSync.
	 * Result of this listening doesn't matter, slot is over and we proceed whether we heard sync keeping sync or not.
	 */
	Phase::set(PhaseEnum::SyncListenSecondHalf);
	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotEnd);

	// not assert Ensemble::isLowPower()
}




// XXX try doing part of the sync slot i.e. fail after the first half because power is exhausted.
// XXX if we heard a MergeSync in the first half, no point in listening for the second half. (minor optimization.)

