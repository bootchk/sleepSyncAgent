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

static void log(char* string) {};

static bool isQueuedWorkMsg(){ return true;}

// time type must be same as DeltaTime
static void scheduleTask(void callback(), uint32_t time ) {}
