#include <cassert>
#include "longClock.h"



// static singleton data members
uint32_t LongClock::mostSignificantBits = 0;
OSTime LongClock::previousOSClockTicks;	// least significant


void LongClock::reset(){
	mostSignificantBits = 1;	// aka epoch
	previousOSClockTicks = OSClockTicks();
	// assert nowTime() < max 32-bit int, but not zero.
}


/*
 * Called more often than OSClock wraps (rolls over.)
 */
LongTime LongClock::nowTime() {

	// Account for quiet wrapping of OSClock (we are not notified by OS.)
	OSTime currentOSClockTicks = OSClockTicks();
	if (currentOSClockTicks < previousOSClockTicks) {
		// OSClock wrapped
		mostSignificantBits++;	// Tick most significant bits
	}
	// ??? Don't fetch OSClockTicks() again, we need the old value
	previousOSClockTicks = currentOSClockTicks;

	// Concatenate MSB and LSB.  Portable?
	LongTime result = mostSignificantBits;
	result = result << OSClockCountBits;	// Left shift result, fill LSB with zero
	result = result | currentOSClockTicks;	// Bit-wise OR into LSB
	return result;
};


/*
 * LongTime math
 *
 * These return shorter uints as returned by OSClock and used by other OS methods.
 *
 * !!! C++ is mind-numbingly stupid: no warnings about coercions with loss?
 */
// TODO more assertions to prevent coercions with loss

/*
 * Not require laterTime is after earlierTime
 * Returns forward time difference when laterTime after earlierTime.
 * Returns zero when laterTime before earlierTime.
 * Requires difference < MaxDeltaTime.
 */
DeltaTime LongClock::clampedTimeDifference(LongTime laterTime, LongTime earlierTime) {
	//
	DeltaTime result;
	if (earlierTime > laterTime)
		result = 0;
	else
		result = laterTime - earlierTime;	// !!! Coerce to 32-bit, with possible loss
	assert(result >= 0);
	// TODO move this assertion to caller
	// assert(result < 3 * PeriodDuration);	// Sanity, app does not schedule far in the future.
	assert(result < MaxDeltaTime);
	return result;
}

/*
 * Not require futureTime later than now, returns 0 if it is.
 * Requires futureTime less than MaxDeltaTime from now
 */
DeltaTime LongClock::clampedTimeDifferenceFromNow(LongTime futureTime) {
	DeltaTime result = clampedTimeDifference(futureTime, nowTime()); // Coerced to 32-bit with possible loss
	// Already asserted: assert(result < MaxDeltaTime);
	return result;
}

/*
 * Smallest difference of givenTime from now, where:
 * - givenTime may be in the past
 * - givenTime is less than MAX_DELTA_TIME from now.
 */
DeltaTime LongClock::timeDifferenceFromNow(LongTime givenTime) {
	LongTime now = nowTime();
	DeltaTime result;

	// Subtract past time from larger future time, else modulo math gives a large result
	if (now < givenTime)
		result = givenTime - now;	// Unsigned, modulo math, with coercion and possible loss
	else
		result = now - givenTime;
	assert(result < MaxDeltaTime);
	return result;
}
