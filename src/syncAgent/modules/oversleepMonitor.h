
#pragma once

#include "../types.h"	// TimeoutFunc



/*
 * A class for sanity checking.
 * When not debugging, can be conditionally compiled to nil.
 */

class OverSleepMonitor {
public:
	// Call just before sleep.  Records info about start of sleep.
	static void markStartSleep(TimeoutFunc);
	// Call after sleep.  Checks that sleep was not longer than intended.  Side effect record oversleep for debugging.
	static bool checkOverslept();

	// Call at any time, but most useful after a sleep and before sleep ended.
	static unsigned int timeElapsedSinceLastStartSleep();
};
