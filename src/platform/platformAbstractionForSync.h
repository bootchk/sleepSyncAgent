#pragma once



/*
 * platform abstraction layer
 *
 * A platform must define all the functions declared here (including nested .h files.)
 * Note that SyncAgent implementation may include the nested .h files, without including this file.
 */

/*
 * !!! Not assume  an RTOS with task scheduling.
 * The platform may provide basic libraries such as mailbox, timer, etc.
 *
 *
 * When using an RTOS with task scheduling:
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

#include "tickCounter.h"
#include "radio.h"
#include "sleep.h"



/*
 * These API's are optional to implement on platform.
 * Depends on how SyncAgent is configured by definitions in config.h
 */
#include "power.h"
#include "mailbox.h"



