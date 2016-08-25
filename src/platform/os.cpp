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
	return Clock_ticks();
}

bool isQueuedMsg(){
	return !Queue_empty(fooq);
}

void freeMsg(void* msg) {
	ICall_freeMsg(msg);
}

void sleepUntilMsgOrTimeout(OSTime) {
	// This also wakes from inter-task messages on the semaphore???
	//  see simpleBLEBroadcaster.c
	ICall_wait(timeout);
}

void sleepUntilTimeout(timeout) {
	Task_wait(timeout);
}

// From TI_RTOS BLE Developer's Guide
void* unqueueMsg(){
	if (!Queue_empty(appMsgQueue))
	{
		sbpEvt_t *pMsg = (sbpEvt_t*)Util_dequeueMsg(appMsgQueue);
		if (pMsg)
		{
			// TODO this is wrapped in TIRTOS event baggage.
			// unwrap it to a BT message and to an app type Message
			return pMsg;
		}
	}
}

#else

// Stubs for clean compile

OSTime OSClockTicks() { return 1; }

bool isQueuedInMsg(){ return true;}
void freeMsg(void* msg) {}
bool isQueuedWorkOutMsg(){ return false; }
void* unqueueMsg(){ return nullptr; }

void sleepUntilMsgOrTimeout(OSTime) {}
void sleepUntilTimeout(OSTime) {}


#endif
