#pragma once

#include "../syncAgent/types.h"	// DeltaTime

#include <nRF5x.h>  // LongTime

/*
 * Knows math on time.
 *
 * All methods static class methods.
 */
class TimeMath {

public:
	// Arithmetic on LongTime yielding a DeltaTime suitable for timeouts on platform

	static DeltaTime clampedTimeDifference(LongTime laterTime, LongTime earlierTime);
	static DeltaTime clampedTimeDifferenceFromNow(LongTime laterTime);
	static DeltaTime clampedTimeDifferenceToNow(LongTime earlierTime);
	static DeltaTime timeDifferenceFromNow(LongTime givenTime);
	static DeltaTime convertLongTimeToOSTime(LongTime givenTime);
};
