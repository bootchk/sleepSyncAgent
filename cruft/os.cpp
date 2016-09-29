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
			// FUTURE this is wrapped in TIRTOS event baggage.
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

#else
// TIRTOS

// Receiver is a BT GAP Observer Role.  Start discovery session (scan)
void turnReceiverOn() {
	GAPObserverRole_StartDiscovery(foo);	// TODO
}

void turnReceiverOff() {
	GAPObserverRole_CancelDiscovery();
}

void xmit(msg) {
	startAdvertising();
	delay short time
	stopAdvertising();
}

SystemID myID() {
	// 48-bit MAC of radio
	// This implementation's result may vary depending on other things.
	// e.g. whether you called a GAP function to set the ID.
	// Alternatively, you can directly read what is burned in ROM (FCFG)
	// See TIForum "registers to read BT address of cc26xx"
	unit64_t ownAddress;
	GAPRole_GetParameter(GAPROLE_BD_ADDR, &ownAddress);
	return ownAddress;
	// see platform_  ??? function
}

#endif

#ifdef BT_IMPLEMENTATION
	// Payload of an app's message sent using BT Broadcast/Observer (sic advertise)
	static constexpr uint8_t advertData[14] =
	{
	  // Flags; this sets the device to use limited discoverable
	  // mode (advertises for 30 seconds at a time) instead of general
	  // discoverable mode (advertises indefinitely)
	  0x02,   // length of this data
	  66, // GAP_ADTYPE_FLAGS,
	  66, // GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

	  // three-byte broadcast of the data "1 2 3"
	  0x0A,   // length of this data including the data type byte
	  66, // GAP_ADTYPE_MANUFACTURER_SPECIFIC, // manufacturer specific adv data type
	  1,	// app MessageType
	  2,	// app MasterID, 6 bytes, 48-bits same as MAC
	  3,
	  4,
	  5,
	  6,
	  7,
	  8,	// app Offset, 2 bytes, 128k
	  9
	};
#endif

	ifdef PLATFORM_TIRTOS

	#define RAND_MAX UINT_MAX	// TODO match to definition in Util?

	uint32 rand() {
		// High-level access to trng hw
		return Util_GetTRNG();	// TODO trng types match
	}

