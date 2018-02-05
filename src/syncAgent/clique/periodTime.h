
#pragma once

#include "../types.h"


/*
 * Type for specialized DeltaTime
 * that spans from SyncPoint forward to another time within normal sync period.
 * Constrained to be less than NormalSyncPeriodDuration.
 *
 * !!!! Pure class, but one instance declared by CliqueMerger.
 */
class PeriodTime {

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

	/*
	 * Convenience (not a method on instance value.)
	 * From an event's offset in ticks from now, return the periodTime (ticks since start of syncPeriod)
	 * of the event.
	 */
	static DeltaTime convertTickOffset(uint32_t);
};
