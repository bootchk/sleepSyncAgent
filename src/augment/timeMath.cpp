
#include <cassert>
#include "timeMath.h"


/*
 * LongTime math
 *
 * These return shorter uints as returned by OSClock and used by other OS methods.
 *
 * use -Wconversion to get warnings about coercions with loss
 *
 * A bit hacky: uses LongClock static methods.  LongClock instance must be initialized first.
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
	DeltaTime result;

	if (earlierTime > laterTime)
		result = 0;
	else {
		// convert with sanity testing (<24bits)
		result = convertLongTimeToOSTime(laterTime - earlierTime);
	}

	return result;
	// Any stronger assertions, i.e. sanity re schedule must be done by caller
}

/*
 * Not require futureTime later than now, returns 0 if it is.
 * Requires futureTime less than MaxDeltaTime from now
 */
DeltaTime TimeMath::clampedTimeDifferenceFromNow(LongTime futureTime) {
	DeltaTime result = clampedTimeDifference(futureTime, LongClock::nowTime()); // Coerced to 32-bit with possible loss
	// Already asserted: assert(result < MaxDeltaTime);
	return result;
}

#ifdef NOT_USED
/*
 * Note require pastTime earlier than now.
 * Returns zero if pastTime earlier than now.
 *
 * Don't use this when pastTime should be asserted to be an earlier time.
 */
DeltaTime TimeMath::clampedTimeDifferenceToNow(LongTime pastTime) {
	DeltaTime result = clampedTimeDifference(LongClock::nowTime(), pastTime); // Coerced to 32-bit with possible loss
	// Already asserted: assert(result < MaxDeltaTime);
	return result;
}
#endif


// TODO this is same as above
/*
 * Smallest difference of givenTime from now, where:
 * - givenTime may be in the past
 * - givenTime is less than MAX_DELTA_TIME from now.
 */
DeltaTime TimeMath::timeDifferenceFromNow(LongTime givenTime) {
	LongTime now = LongClock::nowTime();
	LongTime result;

	// Subtract past time from larger future time, else modulo math gives a large result
	if (now < givenTime)
		result = givenTime - now;	// Unsigned, modulo math
	else
		result = now - givenTime;

	return convertLongTimeToOSTime(result);
}



DeltaTime TimeMath::convertLongTimeToOSTime(LongTime aTime) {
	/*
	 * Assertion must be first, before the conversion that might lose data.
	 * Assert result is an OSTime, which may be 24-bit.
	 * Comparison is less than OR equal to max value of the type DeltaTime.
	 * The coerce is only to 32-bit type.
	 */
	assert(aTime <= Timer::MaxTimeout);

	// !!! Coerce to 32-bit, with possible loss
	DeltaTime result = (DeltaTime) aTime;

	return result;
}



DeltaTime TimeMath::clampedSubtraction(DeltaTime lhs, DeltaTime rhs){
	DeltaTime result;

	// DeltaTime is unsigned
	if (rhs >= lhs)   result = 0;
	else             result = lhs - rhs;
	return result;
}


DeltaTime TimeMath::elapsed(LongTime earlierTime) {
	LongTime now = LongClock::nowTime();

	// Require earlier time to left on practically infinite linear timeline
	assert(now >= earlierTime);

	LongTime diff = now - earlierTime;

	// Will assert if overflows DeltaTime
	return convertLongTimeToOSTime(diff);
}
