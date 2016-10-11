#pragma once

#include <inttypes.h>

/*
 * Fundamental types used by SyncAgent
 */

/*
 * Type of field of SyncMessage to adjust SyncPoint.
 *
 * Design considerations:
 * This is transmitted over-the-air; to keep packets short, it should be small.
 * But the platform RTC clock (OSTime, typically 24 or 32 bits) limits the max size;
 * DeltaSync shouldn't be any larger than the platform RTC allows for timeouts.
 *
 * The size can be no smaller than determined by a combination of:
 * - resolution of the platform RTC clock (OSClock)
 * - SlotDuration
 * - DutyCycleInverse
 * As illustrated by the following derivation:
 * MaxSyncPeriod == MaxDeltaSync / 2    (we must schedule two SyncPeriods forward)
 * MaxSlotCount = MaxSyncPeriod / SlotDuration
 * MaxDutyCycle == 3 awake slots / MaxSlotCount
 * MaxDutyCycleInverse = 1 / MaxDutyCycle
 *
 * Example design choices:
 * for OSClock resolution==1/32khz and SlotDuration==300 ticks:
 * - 2-bytes, 16-bits, MaxDeltaSync is 64k:  max DutyCyleInverse is 30
 * - 3-bytes, 24-bits, MaxDeltaSync is 16M: max DutyCycleInverse is 10k
 *
 * We use -Wconversion to catch possible loss of data,
 * and the code should be asserting no loss of data.
 */

// Currently we are transmitting 3 bytes of DeltaSync,
// i.e. the primitive type uint32_t is the same as for DeltaTime
typedef uint32_t DeltaSync;


/*
 * Used in scheduling to count slots and periods.
 */
// FUTURE since we aren't enforcing this, we might as well use native int
// FUTURE distinguis PeriodCount from SlotCount
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


