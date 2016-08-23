#pragma once

#include <inttypes.h>

/*
 * OS abstraction layer
 *
 * Assume  an RTOS with task scheduling.
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
typedef uint32_t OSTime;

OSTime OSClockTicks();

bool isQueuedInMsg();
bool isQueuedWorkOutMsg();
void freeMsg(void* msg);


void sleepUntilMsgOrTimeout(OSTime);
void sleepUntilTimeout(OSTime);

