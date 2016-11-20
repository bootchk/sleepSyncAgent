
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
 * FUTURE: When using an RTOS with task scheduling:
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
 * Use CFLAG -Wconversion to get warnings
 */




#include "../config.h"	// might define SYNC_AGENT_IS_LIBRARY


#ifndef SYNC_AGENT_IS_LIBRARY
// Use locally defined header files

// platform must provide :  uint32_t rand()
// std C lib provides
#include <cstdlib>

#include "radio.h"
#include "sleeper.h"
#include "osClock.h"
#include "uniqueID.h"
#include "ledLogger.h"

#include "powerManager.h"
#include "mailbox.h"
#include "logger.h"
#include "types.h"



#else


// Use header files from libraries in project which wedges this library.

// Include newlib for rand()
#include <cstdlib>

// Arrange project CFLAGS += -I/home/bootch/git/nRF5rawProtocol/modules
#include <radio.h>

// -I/home/bootch/git/nRF5rawProtocol/platform
#include <sleeper.h>
#include <uniqueID.h>
#include <osClock.h>
#include <logger.h>
#include <mailbox.h>
#include <powerManager.h>
#include <ledLogger.h>


#endif
