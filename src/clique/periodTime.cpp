
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


DeltaTime PeriodTime::convertTickOffset(uint32_t offset) {
	/*
	 * Offset can be forward or backward in time.
	 * Offset is relative to nowTime().
	 *
	 * TODO caller should adjust for OTA latency.
	 * Not important since any error will be the same for all units.
	 * Only important to user, who may notice the discrepancy
	 * (between button push, and work e.g. LED flash.)
	 */
	return ScheduleParameters::NormalSyncPeriodDuration - (offset % ScheduleParameters::NormalSyncPeriodDuration);
}
