#pragma once

#include "../scheduleParameters.h"


/*
 * Constants used by FishPolicy.
 *
 * In an old design, because of HFXO startup delay, we don't fish every sleeping slot.
 * The new design, using RadioPrelude, should fish even in slots abutting the sync slot.
 */
/*
 * Deficient design:
 * Last sleeping slot could be the last slot i.e. CountSlots.
 * But we don't fish it, since it overlaps/delays start of SyncPeriod.
 * Also, fishing the first slot actually fishes the second sleeping slot,
 * and the first sleeping slot is never fished.
 *
 *
 */
class FishingParameters {
public:

/*
 * Certain build configurations (Debug52 running on NRF52DK USB powered)
 * calls to increment duration.
 * This affects the default;
 */

	static const SlotCount MinSlotsTrollingFishedPerPeriod = 1;

	/*
	 * Subtract to account for HFXO Startup.
	 * E.G. If we fish one slot, we can't start fishing at the last slot
	 * because it would overlap the next SyncSlot.
	 */
	static const SlotCount LastSlotOrdinalToFish = ScheduleParameters::CountSlots
			- MinSlotsTrollingFishedPerPeriod;	// !!!
	static const SlotCount FirstSlotOrdinalToFish = ScheduleParameters::FirstSleepingSlotOrdinal;

	/*
	 * A fish session can be many slots.
	 * If power is unlimited, achieve sync faster with longer trolling fish session.
	 *
	 * These are the min/max values.
	 *
	 * Duration includes one HFXOStartup and one or many VirtualSlotDurations
	 */
	static const DeltaTime DeepFishSessionDurationTicks =
				ScheduleParameters::HFXOStartup
				+ ScheduleParameters::VirtualSlotDuration;

	static const DeltaTime MinTrollingRealFishSessionDurationTicks =
					ScheduleParameters::HFXOStartup
					+ MinSlotsTrollingFishedPerPeriod * ScheduleParameters::VirtualSlotDuration;

	static const DeltaTime MinTrollingFishSessionDurationTicks =
			MinSlotsTrollingFishedPerPeriod * ScheduleParameters::VirtualSlotDuration;

	// TODO calculate from syncperiod and active slots
	static const DeltaTime MaxTrollingRealFishSessionDurationTicks = 1000;


	/*
	 * The number of syncPeriods we deep fish.
	 */
	static const unsigned int CountFishingsPerDeepFishing = 6;
};
