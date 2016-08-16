#include <inttypes.h>
#include "time.h"

/*
64-bit clock with resolution same as os kernel clock (typically mSec.)
Clock wraps in millions of years instead of os clock wraps in days.

Cannot be reset.  If the OSClock can be reset and is reset, this should be too.

App must call getTicks() more often than OSClock wraps (getTicks checks for OSClock wrap.)
*/
class LongClock {
	public:
		static LongTime getTicks();
		static DeltaTime clampedTimeDifference(LongTime laterTime, LongTime earlierTime);
	private:
		static uint32_t mostSignificantBits;
		static uint32_t recentLeastSignificantBits;	// as received from os kernel clock
		
		static DeltaTime previousOSClockTicks;
};
