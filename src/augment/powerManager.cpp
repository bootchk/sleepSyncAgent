
#include <cassert>

#include "../../config.h"	// SYNC_AGENT_CONSERVE_POWER
#include "powerManager.h"
#include "../platform/power.h"	// getVcc



#ifdef SYNC_AGENT_CONSERVE_POWER
uint8_t PowerManager::percentVccMeasure() {
	// Convert Vcc measurement from ADC to a percent
	// FUTURE float and proper constants
	int vccValue = getVcc();
	assert(vccValue < (2^10));
	int result = (getVcc() * VrefInVolts) / 1024 ;
	assert (result >= 0 && result <=100);  // is a percent
	return result;
}
#endif


// Specific to my application, knowing storage capacitor is certain joules and need say 3.0V for reserve
// to keep Vcc from falling below VccMin (1.9V?) of mcu.


bool PowerManager::isPowerForRadio(){
#ifdef SYNC_AGENT_CONSERVE_POWER
	return percentVccMeasure() > 66;
#else
	return true;
#endif
}



