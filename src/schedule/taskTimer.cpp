
#include "taskTimer.h"

// radioSoC
#include <clock/timer.h>

#include "../logging/logger.h"



// TODO eliminate wrapper

namespace {
/*
 * Hack while Timer callback has unused parameters.
 * Wrap to change signature.
 */
Task aTask;

void timerCallback(TimerInterruptReason reason) {
	(void) reason;
	aTask();
}

}	// namespace

void TaskTimer::schedule(Task task, DeltaTime duration) {
	aTask = task;

	// Usually name of task is logged just ahead of this
	Logger::log(":"); Logger::logInt(duration);

	Timer::start(
			First,
			duration,
			timerCallback);	// TODO task
}
