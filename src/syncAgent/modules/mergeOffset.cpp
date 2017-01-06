
#include <cassert>
#include "mergeOffset.h"
#include "../scheduleParameters.h"

namespace {

DeltaTime _offset = 0;

}	// namespace



DeltaTime MergeOffset::get() { return _offset; }

void MergeOffset::set(DeltaTime value) {
		assert(isValidValue(value));
		_offset = value;
	}


bool MergeOffset::isValidValue(DeltaTime value) { return value <= ScheduleParameters::NormalSyncPeriodDuration; }

