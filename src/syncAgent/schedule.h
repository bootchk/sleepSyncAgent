
#pragma once


#include "message.h"
#include "longClock.h"

// time that os accepts for scheduling
typedef uint32_t DeltaTime;


/*
 * Schedule is infinite sequence of periods, aligned with global clock of clique.
 * Period is sequence of slots: sync, work, sleeping, ...., sleeping
 *
 * Sleeping slots may be repurposed for fishing or merging.
 * At most one sleeping slot is repurposed per period
 * (so that use of electrical power is not bursty.)
 *
 * Implementation here is mainly about aligning with global clock.
 *
 * Knowledge (implementation) of slot sequence is spread through SyncAgent.
 * (callbacks know which slot should follow.)
 *
 * The mcu and radio may be sleeping(low-power idle) during any slot, not just sleeping slots.
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
	 *
	 *
	 * Duty cycle is 1% == 1/DutyCycleInverse
	 */
	static const int           DutyCycleInverse = 100;

	/*
	 * OSClock freq is 32khz, tick is 0.03ms
	 * SlotDuration should be at least long enough for one message.
	 * e.g. if Bluetooth, one message is 1msec
	 */
	static const DeltaTime     SlotDuration = 300;	// ~ 10msec

	// Fixed by algorithm design
	static const ScheduleCount FirstSleepingSlotOrdinal = 3;  // Sync, Work, Sleep, ...

	// Derived.  Average of 3 slots radio on (Sync, Work, Fish)
	static const ScheduleCount CountSlots = 3*DutyCycleInverse;



public:
	// Start schedule (long duration sequence of periods)
	void startFreshAfterHWReset();
	void resumeAfterPowerRestored();

	void startPeriod();
	void adjustBySyncMsg(SyncMessage msg);

	// Scheduling slots tasks

	void scheduleStartSyncSlotTask(void callback());
	void scheduleEndSyncSlotTask(void callback());

	// Work slot follows sync without start callback
	void scheduleEndWorkSlotTask(void callback());

	void scheduleStartFishSlotTask(void callback());
	void scheduleEndFishSlotTask(void callback());

	void scheduleStartMergeSlotTask(void callback(), DeltaTime offset);
	// Merge slot ends w/o event, next event is startSyncSlot

	// nowTime is not aligned with slot starts.  Result need not be multiple of slotDuration.
	// Used by CliqueMerger()
	DeltaTime  deltaNowToStartNextSync();
	DeltaTime  deltaStartThisSyncToNow();

private:
	// Times
	LongTime startTimeOfNextPeriod();
	LongTime timeOfThisSyncSlotEnd();	// Of this period
	LongTime timeOfThisWorkSlotEnd();
	LongTime timeOfNextSyncSlotStart();	// Of next period.

	// Arithmetic on LongTime
	static DeltaTime clampedTimeDifference(LongTime laterTime, LongTime earlierTime);
	static DeltaTime clampedTimeDifferenceFromNow(LongTime laterTime);
};
