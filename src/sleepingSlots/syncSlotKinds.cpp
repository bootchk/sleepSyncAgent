
#include "syncWorkSlot.h"

// new RTC task design
#include "../sync/syncSlotProperty.h"

#include "../../network/intraCliqueManager.h"
#include "../../work/workOut.h"

#include "syncSlotSchedule.h"
#include "../../modules/syncSender.h"
#include "../../logging/logger.h"
#include <cassert>
#include "../../syncAgentImp/state/phase.h"

#include "../../globals.h"	// clique
#include "../../clique/clique.h"
#include "../../sleepers/syncSleeperObs.h"

/*
 * Sub types of syncWorkSlot
 *
 * Behavior varies according to what needs to be sent.
 */


void SyncWorkSlot::dispatchSyncSlotKind() {


	SyncSlotKind kind = SyncSlotProperty::decideKind();

	switch(kind) {
	case SyncSlotKind::sendControlSync:
		// satisfies needXmitSync
		doSendingControlSyncWorkSlot();
		IntraCliqueManager::checkDoneAndEnactControl();
		break;
	case SyncSlotKind::sendWorkSync:
		// satisfies needXmitSync
		doSendingWorkSyncWorkSlot();
		break;
	case SyncSlotKind::sendSync:
		doMasterSyncWorkSlot();
		break;
	case SyncSlotKind::listen:
		// isSlave OR (isMaster and not xmitting (coin flip))

		doSlaveSyncWorkSlot();
		Logger::log(" Listens ");
		break;
	}
	// Assert some slot was done, either with a transmit or just listen.
}






void SyncWorkSlot::endListen(){
	// Radio might be in use if we timeout'd while receiving
	Ensemble::stopReceiving();

	// FUTURE we could do this elsewhere, e.g. start of sync slot
	if (!clique.isSelfMaster())
		clique.checkMasterDroppedOut();

	Ensemble::shutdown();
}



void SyncWorkSlot::doSendingControlSyncWorkSlot() {
	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot);
	sendControlSync();
	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotEnd);
}


/*
 * Transmit WorkSync in middle.
 * App has queued work to be sent.
 */
void SyncWorkSlot::doSendingWorkSyncWorkSlot(){
	// assert self is Master

	Phase::set(PhaseEnum::SyncListenFirstHalf);
	(void) doListenHalfSyncWorkSlot(SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot);
	assert(!Ensemble::isRadioInUse());

	// TODO if we heard a work sync??
	sendWorkSync();

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
		sendSync();
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


#ifdef OLD
When work is on demand, avoid sending work if already heard it

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
#endif
