/*
 * OSAL for platform TI-RTOS
 *
 * Implements os.h and radioWrapper.h etc
 *
 * The OS and radio stack are independent.
 *
 * Also, some (random.h) may depend on the target board/chip.
 */
#include "os.h"

#ifdef PLATFORM_TIRTOS

// OS is TI-RTOS with BTLE stack

OSTime OSClockTicks() {
	return 1;
}

bool isQueuedMsg(){
	return !Queue_empty(fooq);
}

void freeMsg(void* msg) {
	ICall_freeMsg(msg);
}

void sleepUntilMsgOrTimeout(OSTime) {
	 ICall_wait(timeout);
}

void sleepUntilTimeout(timeout) {
	Task_wait(timeout);
}

#else

// Stubs for clean compile

OSTime OSClockTicks() { return 1; }

bool isQueuedInMsg(){ return true;}
void freeMsg(void* msg) {}
bool isQueuedWorkOutMsg(){ return false; }

void sleepUntilMsgOrTimeout(OSTime) {}
void sleepUntilTimeout(OSTime) {}


#endif
