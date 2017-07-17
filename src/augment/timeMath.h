#pragma once

#include "../syncAgent/types.h"	// DeltaTime

#include <nRF5x.h>  // LongTime

/*
 * Knows math on time.
 *
 * All methods static class methods.
 *
 *
 * Calculation of DeltaTimes
 *
 * uint32 math is modulo: a delta is correct if laterTime > earlierTime
 * (even if the uint32 clock has wrapped).
 * But is incorrect if earlierTime > laterTime.
 * So if you are calculating a timeout to a future deadline time,
 * and the deadline is already in the past, simple math on uint32 is wrong.
 * Use a LongClock to avoid the issue.
 *
 * Calculating a DeltaTime by differencing LongTimes
 * does a conversion to OSTime i.e. take lower 32 bits of result.
 *
 * A sleep...(0) executes immediately without delay.
 * Thus a DeltaTime into the future (for scheduling) can be 0.
 *
 *
 * Times
 *
 * Methods returning a LongTime return future times, as long as called at appropriate instant
 * and task processing times is smaller than slotDurations.
 * When these return past times, calculation of DeltaTime clamps to zero.
 *
 * Methods returning a LongTime may be called many times during a slot (to schedule a new Timer),
 * not just at the start of a slot.
 * Thus you cannot assert they are greater than nowTime().
 */

class TimeMath {

public:
	// Arithmetic on LongTime yielding a DeltaTime suitable for timeouts on platform

	static DeltaTime clampedTimeDifference(LongTime laterTime, LongTime earlierTime);
	static DeltaTime clampedTimeDifferenceFromNow(const LongTime laterTime);
	static DeltaTime clampedTimeDifferenceToNow(LongTime earlierTime);
	static DeltaTime timeDifferenceFromNow(LongTime givenTime);

	/*
	 * Difference of DeltaTimes.
	 * Clamped to zero if rhs is larger than leftHandSize
	 */
	static DeltaTime clampedSubtraction(DeltaTime lhs, DeltaTime rhs);

	/*
	 * Elapsed DeltaTime since earlierTime, referenced to now.
	 * Asserts if earlierTime is after now.
	 * Asserts if difference between earlierTime and now is larger than max DeltaTime.
	 *
	 * Use this when LongTime was simply recorded in the past (not the result of math.)
	 */
	static DeltaTime elapsed(LongTime earlierTime);

private:
	/*
	 * Convert LongTime (typically result of difference) to OSTime, asserting no loss of data
	 */
	static DeltaTime convertLongTimeToOSTime(LongTime givenTime);


};
