
#include "syncWorkSlot.h"

#include "../../modules/syncPowerManager.h"
#include "../../logging/logger.h"
#include "../../logging/flashLogger.h"
#include "../../sleepers/syncSleeperObs.h"

/*
 * Sync slot is three subslots:
 * - first half
 * - sending
 * - second half
 */


HandlingResult SyncWorkSlot::doListenHalfSyncWorkSlot(TimeoutFunc timeoutFunc) {

	/*
	 * This IF is mainly for debugging?
	 * Starting the HFXO (previously) or anything else that preceded this call
	 * is not expected to exhaust power.
	 */
	if (SyncPowerManager::isPowerForSyncSlot()) {
		Ensemble::startReceiving();
	}
	else {
		FlashLogger::logNoPowerForHalfSyncSlot();
		// Note HFXO is still running
		// Continue to sleep for half a slot: we may yet xmit sync, and/or listen for second half.
	}

	HandlingResult result = SyncSleeper::sleepUntilMsgAcceptedOrTimeout(
			SyncSlotMessageHandler::handle,
			timeoutFunc
			);

	/*
	 *  not assert Ensemble::isLowPower():
	 *  The radio might be in use (receiving.)
	 *  The continuation might be send sync and listen another half.
	 *  That continuation needs the radio.
	 */
	return result;
}


/*
 * Did we already hear a Sync message for our clique?
 *
 * Might have heard:
 * - a sync from a worse clique (result KeepListening)
 * - OR a synch-keeping msg (Master, Merge, or WorkSync)
 * - garbled messages or non-sync keeping messages (result KeepListening)
 * - AbandonMastership (from some confused clique that overlaps ours?)
 *
 * !!! A WorkSync might be from a slave, but it briefly keeps sync for our clique,
 * and we omit one MasterSync but don't relinquish Mastership.
 */
bool SyncWorkSlot::didFirstHalfHearSync(HandlingResult firstHalfResult){
	bool result;
	switch (firstHalfResult) {

	case HandlingResult::StopListeningHeardMasterSync:
	case HandlingResult::StopListeningHeardMergeSync:
	case HandlingResult::StopListeningHeardWorkSync:
		result = true;
		break;

	case HandlingResult::TimedOut:
	case HandlingResult::KeepListening:
	default:
		result = false;
		break;
	}
	return result;
}

