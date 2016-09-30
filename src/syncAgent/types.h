#pragma once

#include <inttypes.h>

/*
 * Fundamental types used by SyncAgent
 */

/*
 * Time offset.  2-bytes, 16-bits, 128k
 * Content of sync msg to adjust sync.
 */
typedef uint16_t SyncOffset;


/*
 * Used in scheduling to count slots and periods.
 */
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
// TODO use this in some assertion
static const uint16_t MaximumScheduleCount = 32767;	// !!! Same as std C RAND_MAX
