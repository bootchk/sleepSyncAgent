
#pragma once

#include <cassert>

#include "scheduleParameters.h"

/*
 * Type for specialized DeltaTime that spans from SyncPoint to time to transmit a MergeSync.
 *
 * Constrained to be less than NormalSyncPeriodDuration.
 */
class MergeOffset {
private:
	DeltaTime _offset;

public:
	// Need constructors else can't define instances without "error: use of deleted function"
	MergeOffset() { _offset = 0; }
	MergeOffset(int value) { this->set(value); }

	/*
	 * Returns constrained DeltaTime.
	 * Constrained when set, no assertion thrown on get().
	 */
	DeltaTime get() { return _offset; }

	/*
	 * Constrained: throws assertion if out of range
	 */
	void set(DeltaTime value) {
		assert(isValidValue(value));
		_offset = value;
	}

	// Preflight check value not out of range
	static bool isValidValue(DeltaTime value) { return value <= ScheduleParameters::NormalSyncPeriodDuration; }
};
