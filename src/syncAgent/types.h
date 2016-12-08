#pragma once

#include <inttypes.h>

/*
 * Fundamental types defined and used by SyncAgent.
 * Private to SyncAgent; other types shared with platform should not be here.
 */




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


