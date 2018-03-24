
#include "timer.h"

// TODO, timout func takes a constant, calcs time, every call calculates new delta

void Timer::schedule(Task task, DeltaTime duration) {

	/*
	 * This is intermediate, throwaway implementation
	 */
	DeltaTime aDuration = duration;
	(void) aDuration;
	//SyncSleeper::sleepUntilTimeout(timeoutFunc);
	task();

	/*
	 * Real implementation:
	 *
	 * Timer::setISR(task);
	 * Timer::start();
	 * continue to tight, do-nothing sleep loop
	 */
}
