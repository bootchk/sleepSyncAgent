#include <inttypes.h>
#include "time.h"

/*
64-bit clock with resolution same as os kernel clock (typically mSec.)
Clock wraps in millions of years instead of os clock wraps in days.

Can be reset, but not to zero.  If the OSClock can be reset and is reset, this should be too.

App must call getTicks() more often than OSClock wraps (getTicks checks for OSClock wrap.)

Implementation:
MSB 32-bits lazily incremented by self (only when nowTime() is called)
LSB 32-bits kept by the OS clock

An alternative implementation is to set a task on the OSClock wrap.
When it wraps, increment MSB.
Instead, every call to nowTime() we check for OSClock wrap.
*/
class LongClock {
private:
	static uint32_t mostSignificantBits;
	static uint32_t recentLeastSignificantBits;	// as received from os kernel clock

	static DeltaTime previousOSClockTicks;

public:
	static void reset();

	// getters
	static LongTime nowTime();
	static DeltaTime clampedTimeDifference(LongTime laterTime, LongTime earlierTime);
	static DeltaTime clampedTimeDifferenceFromNow(LongTime laterTime);

};
