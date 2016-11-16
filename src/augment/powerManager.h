

#pragma once

#include <inttypes.h>


/*
 * Understands:
 * - apps power requirements
 * - systems power supply (not a battery, a capacitor.)
 *
 * Here, work takes more power than sync (which requires radio.
 *
 * The work is driving a motor e.g.
 * The work neednot require the radio.
 *
 * Depends on OSAL power manager (ADC, or AON Battery Monitor, etc.)
 */
class PowerManager {

public:

	static bool isPowerForRadio();
	// FUTURE static bool isPowerForWork();

private:
	static int percentVccMeasure();
	// static convertVoltageToPercent(uint32_t voltage);
};
