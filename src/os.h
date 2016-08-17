
#include <inttypes.h>
#include "syncAgent/message.h"

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

//static void sleep() {}
static void turnReceiverOnWithCallback(void callback(Message) ) {}
static void turnReceiverOff() {}
static void xmit(Message msg) {}
static int myID() { return 1;}	// TI-RTOS see platform_  MAC of my radio
static bool isQueuedWorkMsg(){ return true;}

// time type must be same as DeltaTime
static void scheduleTask(void callback(), uint32_t time ) {}
