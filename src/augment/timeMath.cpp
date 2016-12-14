
#include <cassert>
#include "timeMath.h"


/*
 * LongTime math
 *
 * These return shorter uints as returned by OSClock and used by other OS methods.
 *
 * use -Wconversion to get warnings about coercions with loss
 *
 * A bit hacky: uses LongClockTimer static methods.  LongClockTimer instance must be initialized first.
 */

/*
 * Not require laterTime is after earlierTime
 * Returns forward time difference when laterTime after earlierTime.
 * Returns zero when laterTime before earlierTime.
 * Requires difference < MaxDeltaTime.
 *
 * !!!! Note the parameters are not in time order, the laterTime is first parameter
 */
DeltaTime TimeMath::clampedTimeDifference(LongTime laterTime, LongTime earlierTime) {
	//
	LongTime longTimeResult;
	if (earlierTime > laterTime)
		longTimeResult = 0;
	else {
		longTimeResult = laterTime - earlierTime;
	}

	return convertLongTimeToOSTime(longTimeResult);
	// Any stronger assertions, i.e. sanity re schedule must be done by caller
}

/*
 * Not require futureTime later than now, returns 0 if it is.
 * Requires futureTime less than MaxDeltaTime from now
 */
DeltaTime TimeMath::clampedTimeDifferenceFromNow(LongTime futureTime) {
	DeltaTime result = clampedTimeDifference(futureTime, LongClockTimer::nowTime()); // Coerced to 32-bit with possible loss
	// Already asserted: assert(result < MaxDeltaTime);
	return result;
}
DeltaTime TimeMath::clampedTimeDifferenceToNow(LongTime pastTime) {
	DeltaTime result = clampedTimeDifference(LongClockTimer::nowTime(), pastTime); // Coerced to 32-bit with possible loss
	// Already asserted: assert(result < MaxDeltaTime);
	return result;
}

/*
 * Smallest difference of givenTime from now, where:
 * - givenTime may be in the past
 * - givenTime is less than MAX_DELTA_TIME from now.
 */
DeltaTime TimeMath::timeDifferenceFromNow(LongTime givenTime) {
	LongTime now = LongClockTimer::nowTime();
	LongTime result;

	// Subtract past time from larger future time, else modulo math gives a large result
	if (now < givenTime)
		result = givenTime - now;	// Unsigned, modulo math
	else
		result = now - givenTime;

	return convertLongTimeToOSTime(result);
}


/*
 * Convert LongTime to OSTime, asserting no loss of data
 */
DeltaTime TimeMath::convertLongTimeToOSTime(LongTime aTime) {
	// !!! Coerce to 32-bit, with possible loss
	assert(aTime < UINT32_MAX);
	DeltaTime result = (DeltaTime) aTime;
	// Even stronger assertion that result is less than OSTime, which may be 24-bit
	assert(result < LongClockTimer::MaxTimeout);
	return result;
}
