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
	// pass function to sleeper
	syncSleeper.sleepUntilTimeout(fishSchedule.deltaToSlotStart);
}

} //namespace





void FishSlot::perform() {
	// FUTURE: A fish slot need not be aligned with other slots, and different duration???

	// Sleep ultra low-power across normally sleeping slots to start of fish slot
	assert(!radio->isPowerOn());

	network.preamble();

	fishSchedule.init();	// Calculate start time once

	sleepUntilFishSlotStart();

	// logInt(Schedule::deltaPastSyncPointToNow()); log("fish tick\n");

	network.prepareToTransmitOrReceive();
	assert(radio->isPowerOn());

	network.startReceiving();
	assert(!radio->isDisabledState());

	// assert can receive an event that wakes imminently: race to sleep
	syncSleeper.sleepUntilMsgAcceptedOrTimeout(
			&msgHandler,
			fishSchedule.deltaToSlotEnd);
	assert(radio->isDisabledState());
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
	// radio might be off already
	network.shutdown();

	network.postlude();
}


