
#include <nRF5x.h>  // logger
#include <cassert>

#include "deltaSync.h"
#include "../scheduleParameters.h"



DeltaTime DeltaSync::get() const { return _deltaSync; }

// Throws assertion if out of range
void DeltaSync::set(DeltaTime value){
	assert(isValidValue(value));
	_deltaSync = value;
}

// Preflight check value not out of range
bool DeltaSync::isValidValue(DeltaTime value) {
	bool result = value <= ScheduleParameters::NormalSyncPeriodDuration;
	if (!result) {
		logInt(value);
		log("<<<<Invalid OTA offset\n");
	}
	return result;
}
