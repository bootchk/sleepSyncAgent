
#include "../info/infoSlot.h"

#include <ensemble/ensemble.h>

#include "../../modules/syncSender.h"
#include "../../sleepers/scheduleSleeper.h"




// TODO Obsolete
// new is SyncSchedule::syncSendTask();



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
	 * Transmit in middle of sync slot to maximize likelihood of success.
	 *
	 * First deep sleep half a slot (requires ensemble shutdown.)
	 * Then sleep a slot during ensemble startup.
	 * Result is that xmit is in middle of real slot.
	 */
	ScheduleSleeper::sleepUntilSyncSlotMiddle();

	/*
	 * Must succeed even if Ensemble already started.
	 *
	 * Blocking sleep.
	 */
	Ensemble::startup();
	// Ensemble ready but not in use (not receiving.)

	//Phase::set(PhaseEnum::Merge);
	//Logger::logMsgTime();
	SyncSender::sendInfo(item);	// Synchronous

	// Radio is not in use.  Ensemble::shutdown requires it
	Ensemble::shutdown();
}
