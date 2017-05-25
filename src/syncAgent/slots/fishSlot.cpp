/*
 * THE fishSlot of my schedule.
 *
 * Every unit can fish (if not busy merging.)
 * Fish for other cliques by listening.
 */

#include <cassert>


#include "../globals.h"	// syncAgent etc.

#include "fishSlot.h"

#include "fishSchedule.h"
#include "../messageHandler/messageHandler.h"


namespace {

FishSchedule fishSchedule;
FishSlotMessageHandler msgHandler;


void sleepUntilFishSlotStart() {
	// pass to sleeper: function to calculate start time
	syncSleeper.sleepUntilTimeout(fishSchedule.deltaToSlotStart);
}

} //namespace


/*
 * If power, perform
 */
void FishSlot::tryPerform() {
	// FUTURE: A fish slot need not be aligned with other slots, and different duration???

	// Sleep ultra low-power across normally sleeping slots to start of fish slot.
	// Note sleep might have assertions on power condition
	fishSchedule.init();	// Calculate start time once

	sleepUntilFishSlotStart();
	// Power might be replenished.

	if (powerManager->isPowerForRadio()) {
		perform();
	}
	else {
		LogMessage::logNoPowerToFish();
	}
}

/*
 * Listen up to an entire slot.
 * Returns before end of slot if catch another clique.
 */
void FishSlot::perform() {

	// logInt(Schedule::deltaPastSyncPointToNow()); log("fish tick\n");

	network.startup();

	network.startReceiving();

	// assert can receive an event that wakes imminently: race to sleep
	(void) syncSleeper.sleepUntilMsgAcceptedOrTimeout(
			&msgHandler,
			fishSchedule.deltaToSlotEnd);

	// Not using result, might return sooner if caught something

	assert(!network.isRadioInUse());
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

	network.shutdown();
}


