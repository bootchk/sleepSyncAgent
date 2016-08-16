
#include "time.h"
#include "longClock.h"



// OS
DeltaTime OSClockTicks() { return 1; }

// static singleton data members
uint32_t LongClock::mostSignificantBits = 0;
uint32_t LongClock::recentLeastSignificantBits = 0;	// as received from os kernel clock
uint32_t LongClock::previousOSClockTicks;

/*
 * Called more often than OSClock wraps (rolls over.)
 */
LongTime LongClock::getTicks() {

	// Account for quiet wrapping of OSClock (we are not notified by OS.)
	DeltaTime currentOSClockTicks = OSClockTicks();
	if (currentOSClockTicks < previousOSClockTicks) {
		// OSClock wrapped
		mostSignificantBits++;	// Tick most significant bits
	}
	previousOSClockTicks = currentOSClockTicks;

	// Concatenate MSB and LSB.  Portable?
	LongTime result = mostSignificantBits;
	result = result << 32;	// Left shift, fill LSB with zero
	result = result | currentOSClockTicks;	// Bit-wise OR into LSB
	return result;
};

// Can be called if you are not sure laterTime is after earlierTime
DeltaTime LongClock::clampedTimeDifference(LongTime laterTime, LongTime earlierTime) {
	// Returns positive time difference or zero
	DeltaTime result;
	if (earlierTime > laterTime) result = 0;
	else result = laterTime - earlierTime;	// Coerce result
	assert(result >= 0);
	return result;
}
