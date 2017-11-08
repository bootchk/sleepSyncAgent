#pragma once

#include "../../scheduleParameters.h"


/*
 * Constants used by FishPolicy.
 *
 * Now, because of HFXO startup delay, we don't fish every sleeping slot.
 * TODO, improve the design so that we fish every sleeping slot.
 */
/*
 * Deficient design:
 * Last sleeping slot could be the last slot i.e. CountSlots.
 * But we don't fish it, since it overlaps/delays start of SyncPeriod.
 * Also, fishing the first slot actually fishes the second sleeping slot,
 * and the first sleeping slot is never fished.
 */
class FishingParameters {
public:

#ifdef EXTENDED_FISH_DURATION
	static const ScheduleCount SlotsFishedPerPeriod = 20;
#else
	static const ScheduleCount SlotsFishedPerPeriod = 1;
#endif

	/*
	 * Subtract to account for HFXO Startup.
	 * E.G. If we fish one slot, we can't start fishing at the last slot
	 * because it would overlap the next SyncSlot.
	 */
	static const ScheduleCount LastSlotOrdinalToFish = ScheduleParameters::CountSlots
			- SlotsFishedPerPeriod;	// !!!
	static const ScheduleCount FirstSlotOrdinalToFish = ScheduleParameters::FirstSleepingSlotOrdinal;

	/*
	 * A fish session can be many slots.
	 * If power is unlimited, achieve sync faster with longer fish session.
	 *
	 * Certain build configurations (Debug52 running on NRF52DK USB powered)
	 * define EXTENDED_FISH_DURATION
	 *
	 * Duration includes one HFXOStartup and one or many VirtualSlotDurations
	 */
		static const ScheduleCount FishSessionDuration =
				ScheduleParameters::HFXOStartup
				+ SlotsFishedPerPeriod * ScheduleParameters::VirtualSlotDuration;


	static const unsigned int CountFishingsPerDeepFishing = 6;
};
