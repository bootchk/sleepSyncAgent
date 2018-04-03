
#pragma once

// embeddedMath
#include <timeTypes.h>	// DeltaTime



/*
 * Knows how to calculate a sync offset,
 * time from not to next sync point,
 * adjusted for latencies.
 */
class SyncOffset {
public:
	static DeltaTime calculate();
};
