#include <inttypes.h>
#include "time.h"

/*
64-bit clock with resolution same as os kernel clock (typically mSec.)
Clock wraps in millions of years instead of os clock wraps in days.
*/
class LongClock {
	public:
		static void tick();
		static LongTime getTicks();
		static DeltaTime clampedTimeDifference(LongTime laterTime, LongTime earlierTime);
	private:
		static uint32_t mostSignificantBits;
		static uint32_t recentLeastSignificantBits;	// as received from os kernel clock
};
