#pragma once

#include <inttypes.h>

/*
 * Fundamental types defined and used by SyncAgent.
 * Private to SyncAgent; other types shared with platform should not be here.
 */


/*
 * Type used for OTA durations, in ticks of the LongClock.
 *
 * Restrict TimeMath results to this type.
 *
 * Difference of LongTime actually yields another LongTime (modulo math on unsigned.)
 * But the application only needs much smaller results,
 * and TimeMath enforces that.
 */
typedef uint32_t DeltaTime;


/*
 * Index of slot within syncPeriod.
 * Used in scheduling.
 */
// FUTURE enforce not exceed CountSlots
typedef uint32_t SlotCount;


/*
 * Counts events
 * e.g. missed sync
 */
typedef unsigned int EventCount;


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
 * Callback function taking void, returning void.
 */
typedef void (*Callback)();





