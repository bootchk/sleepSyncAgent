#pragma once

#include <cassert>

#include <radioSoC.h>  // LongTime

#include "../types.h"	// ScheduleCount





/*
 * After reset(), fans outward in both directions from current SyncSlot.
 * Try to quickly find a drifted Master.
 *
 * All units may do the same thing.
 * It does not cause contention, because fishing is receiving.
 * However, when all units recover the same Master,
 * then there could be contention to MergeSync.
 * So elsewhere the design should avoid that contention.
 *
 * In this example, the numbers are the ordinals of the slots,
 * where there are 8 slots and slot 1 is the SyncSlot.
 * Yields 2,8,3,7,4,6,5,5,6,4,7,3,8,2,2,8,3,...
 *
 * Note there are two places where the same slot is fished consecutively.
 *
 * !!! Note it is vital to start at the first sleeping slot past and before the SyncSlot.
 * That is where a drifted master is most likely to be.
 */
class SyncRecoveryTrollingPolicy {
public:
	static void restart();
	static bool checkDone();

	/*
	 * There is no init.
	 * currentSessionStartSlotOrdinal and isFish<Foo> are valid even without a call to proceedToNextFishSlotOrdinal().
	 * Said methods are usually called before getStartTimeToFish,
	 */
	static SlotCount currentSessionStartSlotOrdinal();
	static SlotCount currentSessionEndSlotOrdinal();

	/*
	 * Is current fish session in given position in syncperiod.
	 * Near means: within one slot
	 */
	static bool isFishSessionNearStartSyncPeriod();
	static bool isFishSessionNearEndSyncPeriod();


	static LongTime getStartTimeToFish();
	static DeltaTime getFishSessionDuration();

	/*
	 * Subclass specialization
	 */
	static void proceedToNextFishSlotOrdinal();

	static SlotCount getInitialUpCounter();
	static SlotCount getInitialDownCounter();

private:
	static bool isCoverFirstSleepingSlot();
	static bool isAbutFirstSleepingSlot();
	static bool isCoverLastSleepingSlot();
	static bool isAbutLastSleepingSlot();
};




class DeepFishingPolicy {
public:
	static void restart(DeltaTime, Callback);
	static bool checkDone();

	static SlotCount currentSessionStartSlotOrdinal();
	static SlotCount currentSessionEndSlotOrdinal();

	static LongTime getStartTimeToFish();
	static DeltaTime getFishSessionDuration();

	static bool isFishSlotStartSyncPeriod();
	static bool isFishSlotEndSyncPeriod();
};
