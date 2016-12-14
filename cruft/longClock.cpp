#include <cassert>
#include "longClock.h"



// static singleton data members
uint32_t LongClock::mostSignificantBits = 0;
OSTime LongClock::previousOSClockTicks;	// least significant
OSClock LongClock::osClock;


void LongClock::reset(){
	mostSignificantBits = 1;	// aka epoch
	previousOSClockTicks = osClock.ticks();
	// assert nowTime() < max 32-bit int, but not zero.
}


/*
 * Called more often than OSClock wraps (rolls over.)
 */
LongTime LongClock::nowTime() {

	// Account for quiet wrapping of OSClock (we are not notified by OS.)
	OSTime currentOSClockTicks = osClock.ticks();
	if (currentOSClockTicks < previousOSClockTicks) {
		// OSClock wrapped
		log("Wrapped\n");
		logInt(previousOSClockTicks); log("Previous\n");
		logInt(currentOSClockTicks); log("Current\n");

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



