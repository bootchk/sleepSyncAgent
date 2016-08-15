
#include "time.h"
#include "longClock.h"





// static singleton data members
uint32_t LongClock::mostSignificantBits = 0;
uint32_t LongClock::recentLeastSignificantBits = 0;	// as received from os kernel clock

void LongClock::tick() {};
LongTime LongClock::getTicks() {return 1; };

// Can be called if you are not sure laterTime is after earlierTime
DeltaTime LongClock::clampedTimeDifference(LongTime laterTime, LongTime earlierTime) {
	// Returns positive time difference or zero
	DeltaTime result;
	if (earlierTime > laterTime) result = 0;
	else result = laterTime - earlierTime;	// Coerce result
	assert(result >= 0);
	return result;
}
