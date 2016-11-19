
#include <cassert>

#include "../config.h"	// SYNC_AGENT_CONSERVE_POWER
#include "../platform/platform.h"	// PowerManager



#ifdef SYNC_AGENT_CONSERVE_POWER
int PowerManager::percentVccMeasure() {
	// Convert Vcc measurement from ADC to a percent
	// FUTURE proper constants
	int vccValue = getVcc();
	assert(vccValue < (2^10));	// 1024

	// Explicit conversion from double float to int
	int result = (int) ((vccValue / 1024) * 100) ;
	assert (result >= 0 && result <=100);  // is a percent

	return result;
}
#endif


/*
 * Specific to my application:
 *  - storage capacitor is certain joules.
 *  - power supply delivers at most 2.4V (solar cell, depends on light.)
 *  - mcu Vbrownout (aka Vmin) is 1.9V
 *  - VrefInVolts = 3.6V (even if Vcc is less?)
 * Need say 2.1V on capacitor to let radio be turned out without brownout.
 *
 * 2.1/
 */

bool PowerManager::isPowerForRadio(){
#ifdef SYNC_AGENT_CONSERVE_POWER
	return percentVccMeasure() > 66;
#else
	return true;
#endif
}



