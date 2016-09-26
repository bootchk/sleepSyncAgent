
#include <cassert>

#include "powerManager.h"
#include "../platform/power.h"



uint8_t PowerManager::percentVccMeasure() {
	// Convert Vcc measurement from ADC to a percent
	// TODO float and proper constants
	int result = (getVcc() * 3.6) / 1024 ;
	assert (result >= 0 && result <=100);  // is a percent
	return result;
}



// Specific to my application, knowing storage capacitor is certain joules and need say 3.0V for reserve
// to keep Vcc from falling below VccMin (1.9V?) of mcu.

bool PowerManager::isPowerForRadio(){

	return percentVccMeasure() > 66;
}

