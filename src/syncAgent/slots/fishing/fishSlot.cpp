/*
 * THE fishSlot of my schedule.
 *
 * Every unit can fish (if not busy merging.)
 * Fish for other cliques by listening.
 */

#include <cassert>

#include "fishSlot.h"
#include "fishSchedule.h"
#include "fishingManager.h"

#include "../../messageHandler/messageHandler.h"
#include "../../sleepers/syncSleeper.h"
#include "../../modules/syncPowerManager.h"
#include "../../state/phase.h"



namespace {

void sleepUntilFishSlotStart() {
	// pass to sleeper: function to calculate start time
	SyncSleeper::sleepUntilTimeout(FishSchedule::deltaToSlotStart);
}

} //namespace


/*
 * If power, perform
 */
void FishSlot::tryPerform() {
	// FUTURE: A fish slot need not be aligned with other slots, and different duration???

	// Sleep ultra low-power across normally sleeping slots to start of fish slot.
	// Note sleep might have assertions on power condition
	FishSchedule::initStartTime();

	Phase::set(PhaseEnum::SleepTilFish);
	sleepUntilFishSlotStart();
	// Power might be replenished.

	if (SyncPowerManager::isPowerForFishSlot()) {
		// XXX network.start, then test power again
		Phase::set(PhaseEnum::Fish);
		perform();
	}
	else {
		Phase::set(PhaseEnum::NoPowerToFish);
	}
}

/*
 * Listen up to an entire slot.
 * Returns before end of slot if catch another clique.
 */
void FishSlot::perform() {

	// logInt(Schedule::deltaPastSyncPointToNow()); log("fish tick\n");

	Ensemble::startup();

	Ensemble::startReceiving();

	// assert can receive an event that wakes imminently: race to sleep
	(void) SyncSleeper::sleepUntilMsgAcceptedOrTimeout(
			FishSlotMessageHandler::handle,
			FishSchedule::deltaToSlotEnd);

	// Not using result, might return sooner if caught something

	assert(!Ensemble::isRadioInUse());
	Ensemble::shutdown();

	FishingManager::checkFishingDone();

	/*
	 * Conditions:
	 * (no sync msg was heard and receiver still on)
	 * OR (heard a sync msg and (adjusted my schedule OR changed role to Merger))
	 *
	 * In case we adjusted my schedule, now is near its beginning.
	 * The next scheduled sync slot will be almost one full period from now.
	 *
	 * In case we adjusted changed role to Merger,
	 * first mergeSlot will be after next syncSlot.
	 */
}





