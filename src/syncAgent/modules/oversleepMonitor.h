
#pragma once

#include "../types.h"	// TimeoutFunc



/*
 * A class for sanity checking.
 * When not debugging, can be conditionally compiled to nil.
 */

class OverSleepMonitor {
public:
	static void markStartSleep(TimeoutFunc);
	static bool checkOverslept();
	static DeltaTime timeSinceLastStartSleep();
};
