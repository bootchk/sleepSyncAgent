/*
 * OSAL for platform TI-RTOS
 */
#include "os.h"


OSTime OSClockTicks() {
	return 1;
}

bool isQueuedWorkMsg(){
	return true;
}

void sleepUntilMsgOrTimeout(OSTime) {
	/*
	 * ICall_wait(timeout);
	 */
}

void sleepUntilTimeout() {
	/*
	 * Task_wait(timeout);
	 */
}
