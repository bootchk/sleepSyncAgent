
#include "periodTime.h"

#include <cassert>
#include "../scheduleParameters.h"

namespace {

DeltaTime _offset = 0;

}	// namespace



DeltaTime PeriodTime::get() { return _offset; }

void PeriodTime::set(DeltaTime value) {
		assert(isValidValue(value));
		_offset = value;
	}


bool PeriodTime::isValidValue(DeltaTime value) { return value <= ScheduleParameters::NormalSyncPeriodDuration; }

