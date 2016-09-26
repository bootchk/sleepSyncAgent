/*
 * OSAL for platform TI-RTOS
 *
 * Implements os.h and radioWrapper.h etc
 *
 * The OS and radio stack are independent.
 *
 * Also, some (random.h) may depend on the target board/chip.
 */
//#include "platformAbstractionForSync.h"

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


#ifdef PLATFORM_TIRTOS

// TIRTOS use AON BatteryMonitor
// hacked from answer in TI Forum "cc2650 battery monitor measure", from HeartRate example

static uint8_t PowerManager::percentVccMeasure() {
	uint8_t result;

	if (battServiceSetupCB != nullptr) battServiceSetupCB;
	// Read the battery voltage (V), only the first 12 bits
	unit32_t measuredVcc = AONBatMonBatteryVoltageGet();
	result = convertVoltageToPercent(measuredVcc);
	if (battServiceTeardownCB != nullptr) battServiceTeardownCB;
	assert(result <= 100);
	assert(result >= 0);
	return result;
}

uint8_t PowerManager::convertVoltageToPercent(uint32_t voltage) {
	// voltage is 12 bits in units of V, fractional part in lower 8 bits.
	// Convert to from V to mV to avoid fractions.
	// Fractional part is in the lower 8 bits thus converting is done as follows:
	// (1/256)/(1/1000) = 1000/256 = 125/32
	// This is done most effectively by multiplying by 125 and then shifting
	// 5 bits to the right.
	voltage = (voltage * 125) >> 5;
	// Convert to percentage of maximum voltage.
	return ((voltage* 100) / battMaxLevel);
}

#else
