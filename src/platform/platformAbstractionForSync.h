#pragma once

#include <inttypes.h>

/*
 * platform abstraction layer
 *
 * !!! Not assume  an RTOS with task scheduling.
 *
 * Assumes an idle (lowest priority) task that puts system in low-power sleep
 * until a timer wakes up for the next scheduled task
 * or until an interrupt from a device.
 * Convention: any method named "onFoo" is a task.  The system sleeps after the method end.
 */

/*
 * !!!
 * type of 'time' parameter must be same as DeltaTime.
 * Be careful passing a long long, LongTime, or other 64-bit type as compiler
 * will (possibly without warning) convert with possible loss of data (typically lower 32-bits)
 * but only defined by the compiler implementation!!! not by the C standard.
 */


// platform must provide :  uint32_t rand()
// std C lib provides suitable implementation
#include <cstdlib>


typedef uint32_t OSTime;

OSTime OSClockTicks();

/*
 * Queue protocol: queue a pointer, returns a pointer, caller must free what is pointed to.
 */
bool isQueuedInMsg();
bool isQueuedWorkOutMsg();
void freeMsg(void* msg);
void* unqueueMsg();


void sleepUntilMsgOrTimeout(OSTime);
void sleepUntilTimeout(OSTime);

