
#include "powerManager.h"


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
// Stub

uint8_t PowerManager::percentVccMeasure() {
	return 80;
}

#endif


// Specific to my application, knowing storage capacitor is certain joules and need say 3.0V for reserve
// to keep Vcc from falling below VccMin (1.9V?) of mcu.

bool PowerManager::isPowerForRadio(){

	return percentVccMeasure() > 66;
}

