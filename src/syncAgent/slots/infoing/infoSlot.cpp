
#include "infoSlot.h"

#include <ensemble/ensemble.h>

#include "../../modules/syncSender.h"
#include "../../sleepers/syncSleeper.h"

#include "../syncing/syncSlotSchedule.h"

namespace {

void sleepUntilSyncSlotMiddle() {
	SyncSleeper::sleepUntilTimeout(
			SyncSlotSchedule::deltaToThisSyncSlotMiddleSubslot
	);
}

}



void InfoSlot::perform(uint8_t item) {
	/*
	 * Few assertions on sanity of system.
	 *
	 * Starts immediately.
	 *
	 * Do minimum needed to transmit.
	 */

	//Phase::set(PhaseEnum::SleepTilMerge);

	/*
	 * Must succeed even if Ensemble already started.
	 *
	 * Blocking sleep.
	 */
	// TODO check that ensemble can be started twice
	Ensemble::startup();
	// Ensemble ready but not in use (not receiving.)

	// Transmit in middle of sync slot to maximize likelihood of success.
	sleepUntilSyncSlotMiddle();

	//Phase::set(PhaseEnum::Merge);
	//Logger::logMsgTime();
	SyncSender::sendInfo(item);	// Synchronous

	// Radio is not in use.  Ensemble::shutdown requires it
	Ensemble::shutdown();
}
