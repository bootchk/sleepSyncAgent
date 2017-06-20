#pragma once

/*
 * Understands power levels required for SleepSync
 *  *
 * The levels in descending order of power required:
 * - work
 * - sync keeping (radio and clock)
 * - sync maintenance (clock only)
 *
 *
 * For example all the above are duty-cycled (bursts of):
 * - work:  100 mA motor or 20mA LED
 * - sync keeping:  4mA radi0
 * - sync maintenance: mcu
 * The base is the RTC of 1.5uA, which always runs to burst the mcu, etc.
 * If there is not enough power to maintain Vmin for the mcu, brownout and reset occurs.
 *
 * Boot requires a small reserve.
 * Radio requires about 0.2V on capacitor.
 * Radio and work must not brownout mcu.
 * Work requires > 0.2V on capacitor.
 * Radio is required to keep sync.
 * Sync is maintained but drifts if no power for radio.
 * Work may temporarily take Vcc below needed for radio.
 */
#include <nRF5x.h>	// PowerManager

/*
 * Pure class
 */

class SyncPowerManager {

public:
	// Init generic PowerManager
	static void init() { PowerManager::init(); }

	static void enterBrownoutDetectMode() { PowerManager::enterBrownoutDetectMode(); }
	/*
	 * Levels
	 */
	// Above Vmax of chip 3.6V
	static bool isPowerExcess() { return PowerManager::isPowerExcess(); }
	// In sync and can work
	static bool isPowerForWork() { return PowerManager::isPowerAboveHigh(); }	// > 2.7
	// Enough to start a SyncPeriod
	static bool isPowerForSync()  { return PowerManager::isPowerAboveUltraHigh(); }	// > 3.2
	// Enough to continue a SyncPeriod
	static bool isPowerForRadio()  { return PowerManager::isPowerAboveMedium(); } // > 2.5
	// Only enough to count out SyncPeriods, not use radio
	static bool isPowerForIdle()  { return PowerManager::isPowerAboveLow(); }	// > 2.3

	/*
	 * Ranges
	 */
	static VoltageRange getVoltageRange() { return PowerManager::getVoltageRange(); }
};
