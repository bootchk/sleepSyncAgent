
#include "oversleepMonitor.h"


#include "../globals.h"	// Clique
#include "../scheduleParameters.h"


namespace {

// Uses global Sleeper

LongTime sleepStartTime;
DeltaTime intendedSleepDuration;

}


void OverSleepMonitor::markStartSleep(TimeoutFunc timeoutFunc){
	sleepStartTime = clique.schedule.nowTime();
	/*
	 * This was called at start of sleep.
	 * Record initial value of timeoutFunc() monotonic sequence.
	 */
	intendedSleepDuration = timeoutFunc();

	assert(intendedSleepDuration < ScheduleParameters::MaxSaneTimeout);
	// TODO assert <  a more specific, passed saneTimeout
}


bool OverSleepMonitor::checkOverslept(){
	bool result = false;
	if ( timeSinceLastStartSleep() > intendedSleepDuration ) {
		LogMessage::logOverslept();
		result = true;
	}
	return result;
}


DeltaTime OverSleepMonitor::timeSinceLastStartSleep() {
	return (TimeMath::clampedTimeDifferenceToNow(sleepStartTime) ) ;
}
/*
 * TODO also check sanity versus passed in max sane timeout
if ( timeSinceLastStartSleep() > ScheduleParameters::RealSlotDuration ) {
			// Record and try avoid brownout
            LogMessage::logOverslept();
            */
