
#pragma once

#include "message.h"
#include "../../augment/longClock.h"


/*
 * Schedule is infinite sequence of periods, aligned with global clock of clique.
 * Period is sequence of slots: sync, work, sleeping, ...., sleeping
 *
 * Sleeping slots may be repurposed for fishing or merging.
 * At most one sleeping slot is repurposed per period
 * (so that use of electrical power is not bursty.)
 *
 * Division into slots is an ideal.
 * Many sleeping slots go by without any marking by the algorithm.
 * Merge-repurposed slots need not be aligned with my slot divisions
 * (since they are talking to an other units slot divisions.)
 *
 * Implementation here is mainly about aligning with global clock.
 *
 * Knowledge (implementation) of slot sequence is found in syncAgentLoop.cpp, doSyncPeriod().
 *
 * Power:
 * The mcu may be sleeping(low-power idle) during any slot, not just sleeping slots.
 * The radio is off during unrepurposed sleeping slots.
 * The radio is on (xmitting or rcving) during Sync and Work slots.
 * The radio is on (rcving) during Fish repurposed slot.
 * The radio is briefly on (xmitting) during a Merge repurposed slot.
 * The OS schedules tasks using a low-power timer peripheral that never is off.
 *
 * FUTURE message may be received after the intended end of a slot??? or race, msg received after radio off ?
 * (if a msg reception starts during the slot and the radio is not turned off while a msg is in progress.)
 *
 *
 * Responsibilities:
 * - own an infinite duration clock based on OSClock()
 * - maintain period start time (in sync with members of clique)
 * - schedule tasks (interface to OS)
 *
 * Note scheduled tasks run at slot start/end or unaligned
 *
 * !!! Be careful with time math.
 * Compiler will convert longer ints to shorter quietly, with possible loss of data.
 * OS scheduling requires a DeltaTime (known as a timeout in some RTOS) from now.
 * OS scheduling does NOT take a LongTime, but compiler does not give warning
 * (as this code is currently written.)
 *
 * Power:
 * When power is low, SyncAgent returns to app without scheduling next period.
 * When power is restored, SyncAgent's schedule may still be in sync and schedule is resumed.
 * The LongClock continues to be accurate (since it is derived from a hw clock that continues.)
 * Resuming schedule means taking the last known startTimeOfPeriod
 * (which is more than one period in the past)
 * and advancing it to now by a multiple of PeriodDuration.
 */
class Schedule {
private:
	static LongClock longClock;

	static LongTime startTimeOfPeriod;		// updated every period
	static LongTime startTimeOfFishSlot;	// initialized when fish slot starts



	/*
	 * !!! Parameters of schedule
	 * Other params of algorithm at DropoutMonitor.h
	 */

	/*
	 * Ratio of sync period duration to wake duration.
	 * No units, a ratio.
	 *
	 * AKA inverse of "duty cycle"
	 * E.G. 1% DutyCycle same as 100 DutyCycleInverse, sleep about 99% of time
	 *
	 * See types.h MaximumScheduleCount for upper limit:
	 * This affects SyncPeriodDuration,
	 * which cannot be longer than the duration
	 * we can schedule on a Timer provided by OS and RTC hardware.
	 */
	static const int           DutyCycleInverse = 100;

	/*
	 * Duration of all slots.
	 * Units: OSTicks
	 * (When OSClock freq is 32khz, OSTick is 0.03ms)
	 * SlotDuration should > on-air time of a message
	 * since we want to send a message (Sync, Work) within a slot.
	 * e.g. if Bluetooth, one message is ~ 1msec
	 * e.g. if RawWireless, one message is ~0.1msec
	 */
	static const DeltaTime     SlotDuration = 300;	// ~ 10msec

	/*
	 * Fixed by algorithm design.
	 * Sync, Work, Sleep, ..., Sleep
	 */
	static const ScheduleCount FirstSleepingSlotOrdinal = 3;

	/*
	 * Count of slots in sync period.
	 * Must be less than MAX_UINT16 (256k)
	 *
	 * Only used to calculate SyncPeriodDuration
	 * Here 3 is the average count of active slots (with radio on) Sync, Work, Fish.
	 * (Average, since Fish slot is alternative to Merge slot,
	 * which is a short transmit, probablistically transmitted within a SyncPeriod.)
	 */
	static const ScheduleCount CountSlots = 3*DutyCycleInverse;

	static const DeltaTime SyncPeriodDuration = CountSlots * SlotDuration;


// static member funcs
public:
	// Start schedule (long duration sequence of periods)
	static void startFreshAfterHWReset();
	static void resumeAfterPowerRestored();

	static void startPeriod();
	static void adjustBySyncMsg(SyncMessage* msg);


	/*
	 * Deltas from past time to now.
	 */
	// nowTime is not aligned with slot starts.  Result need not be multiple of slotDuration.
	// Used by CliqueMerger()

	static DeltaTime deltaNowToStartNextSync();
	static DeltaTime deltaStartThisSyncPeriodToNow();

	/*
	 * Deltas from now to future time.
	 * Positive or zero and < SyncPeriodDuration
	 */
	static DeltaTime deltaNowToNextSyncPeriod();

	//static DeltaTime deltaToThisSyncSlotStart();
	static DeltaTime deltaToThisSyncSlotEnd();
	static DeltaTime deltaToThisWorkSlotEnd();

	static DeltaTime deltaToThisFishSlotStart();
	static DeltaTime deltaToThisFishSlotEnd();

	static DeltaTime deltaToThisMergeStart(DeltaTime offset);

	/*
	 *  Times
	 */
	static LongTime timeOfNextSyncPeriodStart();
	static LongTime timeOfNextSyncSlotStart();	// Of next period.
	static LongTime timeOfThisSyncSlotEnd();	// Of this period
	static LongTime timeOfThisWorkSlotEnd();
	static LongTime timeOfThisFishSlotStart();
	static LongTime timeOfThisFishSlotEnd();
	static LongTime timeOfThisMergeStart(DeltaTime offset);
};
