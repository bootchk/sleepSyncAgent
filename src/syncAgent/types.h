#pragma once

#include <inttypes.h>

/*
 * Fundamental types defined and used by SyncAgent.
 * Private to SyncAgent; other types shared with platform should not be here.
 */


/*
 * Restrict TimeMath results to this type.
 *
 * Difference of LongTime actually yields another LongTime (modulo math on unsigned.)
 * But the application only needs much smaller results,
 * and TimeMath enforces that.
 */
typedef uint32_t DeltaTime;


/*
 * Used in scheduling to count slots and periods.
 */
// FUTURE since we aren't enforcing this, we might as well use native int
// FUTURE distinguish PeriodCount from SlotCount
typedef uint16_t ScheduleCount;


/*
 * This helps defines the MaximumDutyCycle.
 * !!! MaximumDutyCycle usually smaller, typically 10, 100, or 1024.
 * The limiting factor is use of rand() in randUnsignedInt()
 * which limits the max value to RAND_MAX, typically 32767.
 *
 * Since we are awake for at least Sync and Work slots,
 * duty cycle is MaximumScheduleCount/2
 */
// FUTURE use this in some assertion
static const uint16_t MaximumScheduleCount = 32767;	// !!! Same as std C RAND_MAX


/*
 * Returns a timeout calculated from Schedule.
 * Monotonic: result from consecutive calls is decreasing function down to zero.
 * A function will restart when the schedule advances to another slot or syncperiod.
 */
// ??? Was OSTime defined by nRF5x
typedef DeltaTime (*TimeoutFunc)();


/*
 * Index of debugging flags written to words in UICR.
 *
 * Typically use word as a flag for certain exceptions and other events.
 * A flag is 'set' by writing all zeroes to it.
 *
 * Exceptions are handled generically, writing PC to flash.
 * Flagged events are specific to the application.
 */
enum AppFlagsIndex {
	// First three are reserved by nRF5x library
	//HardFaultPCIndex = 0,			// hw fault PC
	//ExitFaultIndex,
	//LineNumberIndex, 				// line no of assert

	// Normal algorithm step (Phase) written to flash on a fault or brownout warning
	PhaseBOIndex = 3,						// phase of algorithm when fault or brownout occurred
	// Bits per phase written first time phase is done
	PhasesDoneIndex,
	//
	/*
	 *  Flags for exceptions discovered in the code, for robustness.
	 *  Alternative to C++ exception mechanism.
	 */
	UnexpectedWake,					// sleep ended but timer not expired
	UnexpectedMsg,					// Radio IRQ while radio disabled?
	UnexpectedWakeWhileListen,		// radio on but woken for unknown reason

	/*
	 * Rare but expected events.
	 */
	ExcessPowerEventFlagIndex,		// Vcc above 3.6V
	WorkEventFlagIndex,				// Worked e.g. flashed LED
	NoPowerToFish,					// Vcc fell below 2.5V
	NoPowerToStartSyncSlot,			//    "
	NoPowerToHalfSyncSlot,			//  "
	PauseSync,					    // not enough power to listen/send sync
	OverSlept,						//
	IntendedSleepDuration,
	OversleptDuration,			    // overslept sleep time

	// These should not exceed 16, rest reserved for brownout traces and assertion string
};


