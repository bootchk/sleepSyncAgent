
#include "infoSlot.h"

#include "../modules/syncSender.h"
#include <ensemble/ensemble.h>


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

	//Phase::set(PhaseEnum::Merge);
	//Logger::logMsgTime();
	SyncSender::sendInfo(item);	// Synchronous

	// Radio is not in use.  Ensemble::shutdown requires it
	Ensemble::shutdown();
}
