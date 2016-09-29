#include <inttypes.h>
#include "../platform/tickCounter.h"	// OSClockTicks

/*
56-bit OR 64-bit clock with resolution same as os kernel clock (typically 0.03 or 1 mSec.)
Clock wraps in thousands OR millions of years instead of os clock wraps in minutes OR days.


!!! This clock does not have alarms.  On most platforms, see Timer.

Can be reset, but not to zero.  If the OSClock can be reset and is reset, this should be too.

App must call getTicks() more often than OSClock wraps (getTicks checks for OSClock wrap.)

Implementation:
MSB 32-bits lazily incremented by self (only when nowTime() is called)
LSB 24-bits OR 32-bits kept by the OS clock

An alternative implementation is to set a task on the OSClock wrap.
When it wraps, increment MSB.
Instead, every call to nowTime() we check for OSClock wrap.
*/

// FUTURE better enforcement of 24-bit DeltaTime

// long time with same, high resolution as OSClock
typedef uint64_t LongTime;




class LongClock {
private:
	// Components of LongTime
	static uint32_t mostSignificantBits;
	static OSTime recentLeastSignificantBits;	// as received from  platform clock (os kernel or otherwise)

	// Used to detect OSClock rollover
	static OSTime previousOSClockTicks;

public:
	static void reset();

	// getters
	static LongTime nowTime();

	// Arithmetic on LongTime yielding a DeltaTime suitable for timeouts on platform
	static DeltaTime clampedTimeDifference(LongTime laterTime, LongTime earlierTime);
	static DeltaTime clampedTimeDifferenceFromNow(LongTime laterTime);
};
