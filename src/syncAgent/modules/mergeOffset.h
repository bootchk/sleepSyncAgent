
#pragma once

#include "../types.h"


/*
 * Type for specialized DeltaTime that spans from SyncPoint to time to transmit a MergeSync.
 *
 * Constrained to be less than NormalSyncPeriodDuration.
 *
 * Only one instance, owned by CliqueMerger.
 */
class MergeOffset {

public:
	/*
	 * Returns constrained DeltaTime.
	 * Constrained when set, no assertion thrown on get().
	 */
	static DeltaTime get();

	/*
	 * Constrained: asserts if out of range
	 */
	static void set(DeltaTime value);

	// Preflight check value not out of range
	static bool isValidValue(DeltaTime value);
};
