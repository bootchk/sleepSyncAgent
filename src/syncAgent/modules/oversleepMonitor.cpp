
#include "oversleepMonitor.h"


#include "../globals.h"	// Clique, CustomFlash
#include "../scheduleParameters.h"


namespace {

// Uses global Sleeper

LongTime sleepStartTime;
DeltaTime intendedSleepDuration;

}


void OverSleepMonitor::markStartSleep(TimeoutFunc timeoutFunc){
	// Require called at start of sleep.

	/*
	 *  Record time sleep started, more or less.
	 *  Clock may advance by say one before sleep actually starts.
	 */
	sleepStartTime = clique.schedule.nowTime();

	/*
	 * Record initial value of timeoutFunc() monotonic sequence, more or less.
	 * Sleeping will call timeoutFunc again, and that result might decrease by say one in the meantime.
	 */
	intendedSleepDuration = timeoutFunc();

	assert(intendedSleepDuration < ScheduleParameters::MaxSaneTimeout);
	// TODO assert <  a more specific, passed saneTimeout
}


bool OverSleepMonitor::checkOverslept(){
	bool result = false;

	/*
	 * Record sleep duration.
	 * Clock might have advanced by say one since sleep actually stopped (+1).
	 * startTimeOfSleep may be -1 or more.
	 * So this result may be +2 or more.
	 */
	DeltaTime actualSleepDuration = timeSinceLastStartSleep() ;

	/*
	 * 2 is the adjustment for imprecise timekeeping
	 */
	if ( actualSleepDuration > intendedSleepDuration + ScheduleParameters::OversleepMargin ) {
		// Write global sync phase we were sleeping in, if not already written by brownout
		CustomFlash::tryWriteIntAtIndex(PhaseIndex, syncAgent.getPhase());

		CustomFlash::tryWriteIntAtIndex(OversleptDurationIndex, actualSleepDuration);

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
